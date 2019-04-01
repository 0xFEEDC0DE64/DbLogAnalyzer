#pragma once

#include <QThread>
#include <QSqlQuery>
#include <QRegularExpression>

#include "common.h"

class QSqlDatabase;

class ImportThread : public QThread
{
    Q_OBJECT

public:
    ImportThread(QSqlDatabase &database, const ScanResult &result, const QString &timeFormat, QObject *parent = nullptr);

signals:
    void statusUpdate(const QString &message);
    void progressUpdate(qint64 finished, qint64 total);
    void logMessage(const QString &message);

protected:
    void run() override;

private:
    int getHostID(const QString &host);
    int getProcessID(const QString &process);
    int getFilenameID(const QString &filename);
    int getThreadID(const QString &thread);
    int getTypeID(const QString &type);

    qint64 calculateTotalSize() const;

    QSqlDatabase &m_database;
    ScanResult m_result;
    QString m_timeFormat;

    QHash<QString, int> m_hosts;
    QHash<QString, int> m_processes;
    QHash<QString, int> m_filenames;
    QHash<QString, int> m_threads;
    QHash<QString, int> m_types;

    QSqlQuery m_queryInsertLog;
    QSqlQuery m_queryFindHost;
    QSqlQuery m_queryInsertHost;
    QSqlQuery m_queryFindProcess;
    QSqlQuery m_queryInsertProcess;
    QSqlQuery m_queryFindFilename;
    QSqlQuery m_queryInsertFilename;
    QSqlQuery m_queryFindThread;
    QSqlQuery m_queryInsertThread;
    QSqlQuery m_queryFindType;
    QSqlQuery m_queryInsertType;

    const qint64 m_totalSize;
    quint64 m_logsInserted { 0 };
    QDateTime m_lastProgressUpdate;

    const QRegularExpression m_lineRegex { "^(([0-9]+:[0-9]+:[0-9]+(?:\\.[0-9]+)?) (FATAL:   |ERROR:   |WARNING: |INFO:    |DEBUG:   |DEBUG1:  |DEBUG2:  ))(.*)$" };
    const QRegularExpression m_threadRegex { "----- Thread context: (.*) -----$" };
};
