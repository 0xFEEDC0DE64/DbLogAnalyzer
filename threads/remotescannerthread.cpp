#include "remotescannerthread.h"

#include <QDirIterator>
#include <QFileInfo>
#include <QStringBuilder>

RemoteScannerThread::RemoteScannerThread(const QString &dir, QObject *parent) :
    QThread(parent),
    m_dir(dir)
{
}

void RemoteScannerThread::run()
{
    scanForHosts();
}

void RemoteScannerThread::scanForHosts()
{
    if (m_files)
        qFatal("thread was already run");

    QDirIterator iter(m_dir, QDir::Dirs | QDir::NoDotAndDotDot);
    while (iter.hasNext())
    {
        if (isInterruptionRequested())
            return;

        const QFileInfo fileInfo(iter.next());

        const auto host = fileInfo.fileName();

        emit logMessage(tr("Scanning host %0...").arg(host));

        scanForLogfiles(host, fileInfo.absoluteFilePath());
    }

    emit progressUpdate(m_files, m_files - m_valid);
    emit finished(m_result);
}

void RemoteScannerThread::scanForLogfiles(const QString &hostname, const QString &hostDir)
{
    auto &hostEntry = m_result[hostname];

    QDirIterator dirIter(hostDir, { "*.log.gz" }, QDir::Files);
    while (dirIter.hasNext())
    {
        if (isInterruptionRequested())
            return;

        const QFileInfo fileInfo(dirIter.next());

        m_files++;

        const auto match = m_fileExpression.match(fileInfo.fileName());
        if (!match.hasMatch())
            continue;

        const auto process = match.captured(1);
        if (process.endsWith(QStringLiteral(".olog")))
            continue;

        const auto date = QDate::fromString(match.captured(2), QStringLiteral("yyyyMMdd"));
        if (date.isNull())
            continue;

        m_valid++;

        Q_ASSERT(!hostEntry[process].contains(date));

        hostEntry[process].insert(date, {
            hostname % "/" % fileInfo.fileName(),
            fileInfo.absoluteFilePath(),
            fileInfo.size(),
            true
        });

        const auto now = QDateTime::currentDateTime();
        if (m_lastUpdate.isNull() || m_lastUpdate.msecsTo(now) >= 100)
        {
            emit progressUpdate(m_files, m_files - m_valid);
            m_lastUpdate = now;
        }
    }
}
