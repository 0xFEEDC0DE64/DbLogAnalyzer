#include "importthread.h"

#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QVariant>

#include <utility>

#include "gzipdevice.h"

ImportThread::ImportThread(QSqlDatabase &database, const ScanResult &result, const QString &timeFormat, QObject *parent) :
    QThread(parent),
    m_database(database),
    m_result(result),
    m_timeFormat(timeFormat),
    m_queryInsertLog(database),
    m_queryFindHost(database),
    m_queryInsertHost(database),
    m_queryFindProcess(database),
    m_queryInsertProcess(database),
    m_queryFindFilename(database),
    m_queryInsertFilename(database),
    m_queryFindThread(database),
    m_queryInsertThread(database),
    m_queryFindType(database),
    m_queryInsertType(database),
    m_totalSize(calculateTotalSize())
{
}

void ImportThread::run()
{
    {
        int query { 0 };
        typedef std::tuple<QString, QSqlQuery&, QString> Row;
        for (const auto &tuple : {
            Row(tr("insert log"),  m_queryInsertLog,      "INSERT INTO `Logs` (`Timestamp`, `HostID`, `ProcessID`, `FilenameID`, `ThreadID`, `TypeID`, `Message`) "
                                                          "VALUES (:Timestamp, :HostID, :ProcessID, :FilenameID, :ThreadID, :TypeID, :Message);"),
            Row(tr("find host"),   m_queryFindHost,       "SELECT `ID` FROM `Hosts` WHERE `Name` = :Name;"),
            Row(tr("insert host"), m_queryInsertHost,     "INSERT INTO `Hosts` (`Name`) VALUES (:Name);"),
            Row(tr("find host"),   m_queryFindProcess,    "SELECT `ID` FROM `Processes` WHERE `Name` = :Name;"),
            Row(tr("insert host"), m_queryInsertProcess,  "INSERT INTO `Processes` (`Name`) VALUES (:Name);"),
            Row(tr("find host"),   m_queryFindFilename,   "SELECT `ID` FROM `Filenames` WHERE `Name` = :Name;"),
            Row(tr("insert host"), m_queryInsertFilename, "INSERT INTO `Filenames` (`Name`) VALUES (:Name);"),
            Row(tr("find host"),   m_queryFindThread,     "SELECT `ID` FROM `Threads` WHERE `Name` = :Name;"),
            Row(tr("insert host"), m_queryInsertThread,   "INSERT INTO `Threads` (`Name`) VALUES (:Name);"),
            Row(tr("find host"),   m_queryFindType,       "SELECT `ID` FROM `Types` WHERE `Name` = :Name;"),
            Row(tr("insert host"), m_queryInsertType,     "INSERT INTO `Types` (`Name`) VALUES (:Name);")
        })
        {
            if (isInterruptionRequested())
                return;

            emit statusUpdate(tr("Preparing query to %0...").arg(std::get<0>(tuple)));
            if (!std::get<1>(tuple).prepare(std::get<2>(tuple)))
            {
                emit statusUpdate(tr("Failed."));
                emit logMessage(tr("Could not prepare query to %0: %1").arg(std::get<0>(tuple), std::get<1>(tuple).lastError().text()));
                return;
            }
            emit progressUpdate(query++, 11);
        }
    }

    qint64 processedSize { 0 };

    for (auto hostsIter = m_result.constBegin(); hostsIter != m_result.constEnd(); hostsIter++)
    {
        if (isInterruptionRequested())
            return;

        m_queryInsertLog.bindValue(":HostID", getHostID(hostsIter.key()));

        for (auto processesIter = hostsIter.value().constBegin(); processesIter != hostsIter.value().constEnd(); processesIter++)
        {
            if (isInterruptionRequested())
                return;

            m_queryInsertLog.bindValue(":ProcessID", getProcessID(processesIter.key()));

            for (auto datesIter = processesIter.value().constBegin(); datesIter != processesIter.value().constEnd(); datesIter++)
            {
                if (isInterruptionRequested())
                    return;

                m_queryInsertLog.bindValue(":FilenameID", getFilenameID(datesIter.value().filename));

                emit logMessage(datesIter.value().filename);

                QFile file(datesIter.value().filepath);
                QFile::OpenMode flags = QIODevice::ReadOnly;
                if (!datesIter.value().gzipCompressed)
                    flags |= QIODevice::Text;
                if (!file.open(flags))
                {
                    emit logMessage(tr("Could not open logfile: %0").arg(file.errorString()));
                    continue;
                }

                struct {
                    QDateTime dateTime;
                    QString thread { "main" };
                    QString type;
                    QString message;
                } test;

                const auto insert = [&test,this,&processedSize,&file](){
                    m_queryInsertLog.bindValue(":Timestamp", test.dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz")));
                    m_queryInsertLog.bindValue(":ThreadID", getThreadID(test.thread));
                    m_queryInsertLog.bindValue(":TypeID", getTypeID(test.type));
                    m_queryInsertLog.bindValue(":Message", test.message);
                    if (!m_queryInsertLog.exec())
                        emit logMessage(tr("could not execute query to insert log: %0").arg(m_queryInsertLog.lastError().text()));

                    m_logsInserted++;
                    const auto now = QDateTime::currentDateTime();
                    if (m_lastProgressUpdate.isNull() || m_lastProgressUpdate.msecsTo(now) >= 100)
                    {
                        emit statusUpdate(tr("%0 logs inserted...").arg(m_logsInserted));
                        emit progressUpdate(processedSize + file.pos(), m_totalSize);
                        m_lastProgressUpdate = now;
                    }
                };

                int indentionOffset;

                if (!m_database.transaction())
                {
                    emit statusUpdate(tr("Aborted."));
                    emit logMessage(tr("Could not start transaction: %0").arg(m_database.lastError().text()));
                    return;
                }

                try
                {
                    const std::unique_ptr<GzipDevice> gzipProxy([&datesIter,&file](){
                        if (datesIter.value().gzipCompressed)
                            return std::make_unique<GzipDevice>(file);
                        return std::unique_ptr<GzipDevice>{};
                    }());

                    QTextStream textStream(gzipProxy != nullptr ? static_cast<QIODevice*>(gzipProxy.get()) : &file);
                    while(!textStream.atEnd())
                    {
                        if (isInterruptionRequested())
                        {
                            if (!m_database.rollback())
                            {
                                emit statusUpdate(tr("Aborted."));
                                emit logMessage(tr("Could not rollback transaction: %0").arg(m_database.lastError().text()));
                            }
                            return;
                        }

                        auto line = textStream.readLine();
                        const auto match = m_lineRegex.match(line);
                        if (match.hasMatch())
                        {
                            if (!test.dateTime.isNull())
                                insert();

                            test.message = match.captured(4);

                            {
                                const auto threadMatch = m_threadRegex.match(test.message);
                                if (threadMatch.hasMatch())
                                    test.thread = threadMatch.captured(1);
                            }

                            test.dateTime = { datesIter.key(), QTime::fromString(match.captured(2), m_timeFormat) };

                            test.type = match.captured(3);
                            test.type = test.type.left(test.type.indexOf(':'));

                            indentionOffset = match.captured(1).length();
                        }
                        else
                        {
                            if (!test.dateTime.isNull())
                            {
                                if (line.length() >= indentionOffset &&
                                    std::all_of(line.constBegin(), line.constBegin() + indentionOffset, [](const QChar &c){ return c == ' '; }))
                                    line.remove(0, indentionOffset);

                                test.message.append("\n");
                                test.message.append(line);
                            }
                        }
                    }

                    if (!test.dateTime.isNull())
                        insert();
                }
                catch (const std::exception &ex)
                {
                    emit logMessage(tr("Aborted: %0").arg(ex.what()));
                }

                if (!m_database.commit())
                {
                    emit statusUpdate(tr("Aborted."));
                    emit logMessage(tr("Could not commit transaction: %0").arg(m_database.lastError().text()));
                    return;
                }

                processedSize += file.size();
                emit statusUpdate(tr("%0 logs inserted...").arg(m_logsInserted));
                emit progressUpdate(processedSize, m_totalSize);
            }
        }
    }
}

int ImportThread::getHostID(const QString &host)
{
    const auto iter = m_hosts.find(host);
    if (iter != m_hosts.constEnd())
        return *iter;

    m_queryFindHost.bindValue(":Name", host);
    if (!m_queryFindHost.exec())
        qFatal("could not execute query to find host: %s", qPrintable(m_queryFindHost.lastError().text()));

    if (m_queryFindHost.next())
    {
        const auto id = m_queryFindHost.value(0).toInt();
        m_hosts.insert(host, id);
        return id;
    }

    m_queryInsertHost.bindValue(":Name", host);
    if (!m_queryInsertHost.exec())
        qFatal("could not execute query to insert host: %s", qPrintable(m_queryInsertHost.lastError().text()));

    const auto id = m_queryInsertHost.lastInsertId().toInt();
    m_hosts.insert(host, id);
    return id;
}

int ImportThread::getProcessID(const QString &process)
{
    const auto iter = m_processes.find(process);
    if (iter != m_processes.constEnd())
        return *iter;

    m_queryFindProcess.bindValue(":Name", process);
    if (!m_queryFindProcess.exec())
        qFatal("could not execute query to find process: %s", qPrintable(m_queryFindProcess.lastError().text()));

    if (m_queryFindProcess.next())
    {
        const auto id = m_queryFindProcess.value(0).toInt();
        m_processes.insert(process, id);
        return id;
    }

    m_queryInsertProcess.bindValue(":Name", process);
    if (!m_queryInsertProcess.exec())
        qFatal("could not execute query to insert process: %s", qPrintable(m_queryInsertProcess.lastError().text()));

    const auto id = m_queryInsertProcess.lastInsertId().toInt();
    m_processes.insert(process, id);
    return id;
}

int ImportThread::getFilenameID(const QString &filename)
{
    const auto iter = m_filenames.find(filename);
    if (iter != m_filenames.constEnd())
        return *iter;

    m_queryFindFilename.bindValue(":Name", filename);
    if (!m_queryFindFilename.exec())
        qFatal("could not execute query to find filename: %s", qPrintable(m_queryFindFilename.lastError().text()));

    if (m_queryFindFilename.next())
    {
        const auto id = m_queryFindFilename.value(0).toInt();
        m_filenames.insert(filename, id);
        return id;
    }

    m_queryInsertFilename.bindValue(":Name", filename);
    if (!m_queryInsertFilename.exec())
        qFatal("could not execute query to insert filename: %s", qPrintable(m_queryInsertFilename.lastError().text()));

    const auto id = m_queryInsertFilename.lastInsertId().toInt();
    m_filenames.insert(filename, id);
    return id;
}

int ImportThread::getThreadID(const QString &thread)
{
    const auto iter = m_threads.find(thread);
    if (iter != m_threads.constEnd())
        return *iter;

    m_queryFindThread.bindValue(":Name", thread);
    if (!m_queryFindThread.exec())
        qFatal("could not execute query to find thread: %s", qPrintable(m_queryFindThread.lastError().text()));

    if (m_queryFindThread.next())
    {
        const auto id = m_queryFindThread.value(0).toInt();
        m_threads.insert(thread, id);
        return id;
    }

    m_queryInsertThread.bindValue(":Name", thread);
    if (!m_queryInsertThread.exec())
        qFatal("could not execute query to insert thread: %s", qPrintable(m_queryInsertThread.lastError().text()));

    const auto id = m_queryInsertThread.lastInsertId().toInt();
    m_threads.insert(thread, id);
    return id;
}

int ImportThread::getTypeID(const QString &type)
{
    const auto iter = m_types.find(type);
    if (iter != m_types.constEnd())
        return *iter;

    m_queryFindType.bindValue(":Name", type);
    if (!m_queryFindType.exec())
        qFatal("could not execute query to find type: %s", qPrintable(m_queryFindType.lastError().text()));

    if (m_queryFindType.next())
    {
        const auto id = m_queryFindType.value(0).toInt();
        m_types.insert(type, id);
        return id;
    }

    m_queryInsertType.bindValue(":Name", type);
    if (!m_queryInsertType.exec())
        qFatal("could not execute query to insert type: %s", qPrintable(m_queryInsertType.lastError().text()));

    const auto id = m_queryInsertType.lastInsertId().toInt();
    m_types.insert(type, id);
    return id;
}

qint64 ImportThread::calculateTotalSize() const
{
    qint64 totalSize { 0 };
    for (auto hostsIter = m_result.constBegin(); hostsIter != m_result.constEnd(); hostsIter++)
        for (auto processesIter = hostsIter.value().constBegin(); processesIter != hostsIter.value().constEnd(); processesIter++)
            for (auto datesIter = processesIter.value().constBegin(); datesIter != processesIter.value().constEnd(); datesIter++)
                totalSize += datesIter.value().filesize;
    return totalSize;
}
