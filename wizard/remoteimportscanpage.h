#pragma once

#include <QWizardPage>
#include <QPixmap>
#include <QMovie>

#include <memory>

#include "common.h"

class QLabel;
class QPlainTextEdit;

class RemoteScannerThread;

class RemoteImportScanPage : public QWizardPage
{
    Q_OBJECT

    const QPixmap m_pixmapSucceeded { ":/loganalyzer/icons/succeeded.png" };
    const QPixmap m_pixmapFailed { ":/loganalyzer/icons/failed.png" };
    QMovie m_movieLoading { ":/loganalyzer/icons/loading.gif" };

public:
    explicit RemoteImportScanPage(QWidget *parent = nullptr);
    ~RemoteImportScanPage() override;

    int nextId() const override;

    void initializePage() override;
    void cleanupPage() override;
    bool isComplete() const override;

private slots:
    void progressUpdate(int totalFiles, int skippedFiles);
    void logMessage(const QString &message);
    void finished(const ScanResult &result);

private:
    std::unique_ptr<RemoteScannerThread> m_thread;

    QLabel *m_labelAnimation;
    QLabel *m_labelStatus;
    QPlainTextEdit *m_logView;
};
