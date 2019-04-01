#pragma once

#include <QThread>
#include <QRegularExpression>
#include <QSet>
#include <QString>
#include <QDate>

#include "common.h"

class RemoteScannerThread : public QThread
{
    Q_OBJECT

    const QRegularExpression m_fileExpression { "^(.+)-([0-9]{8})\\.log?" };

public:
    RemoteScannerThread(const QString &dir, QObject *parent = nullptr);

signals:
    void progressUpdate(int totalFiles, int skippedFiles);
    void logMessage(const QString &message);
    void finished(const ScanResult &result);

protected:
    void run() override;

private:
    void scanForHosts();
    void scanForLogfiles(const QString &hostname, const QString &hostDir);

    QString m_dir;

    ScanResult m_result;

    int m_files { 0 };
    int m_valid { 0 };

    QDateTime m_lastUpdate;
};
