#pragma once

#include <QWizardPage>
#include <QPixmap>
#include <QMovie>

#include <memory>

class QLabel;
class QProgressBar;
class QPlainTextEdit;

class ImportThread;

class ImportProgressPage : public QWizardPage
{
    Q_OBJECT

    const QPixmap m_pixmapSucceeded { ":/loganalyzer/icons/succeeded.png" };
    const QPixmap m_pixmapFailed { ":/loganalyzer/icons/failed.png" };
    QMovie m_movieLoading { ":/loganalyzer/icons/loading.gif" };

public:
    explicit ImportProgressPage(QWidget *parent = nullptr);
    ~ImportProgressPage() override;

    void initializePage() override;
    void cleanupPage() override;

    int nextId() const override;
    bool isComplete() const override;

private slots:
    void statusUpdate(const QString &message);
    void progressUpdate(qint64 finished, qint64 total);
    void logMessage(const QString &message);
    void finished();

private:
    QLabel *m_labelIcon;
    QLabel *m_labelStatus;
    QProgressBar *m_progressBar;
    QPlainTextEdit *m_logView;

    std::unique_ptr<ImportThread> m_thread;
};
