#include "remoteimportscanpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>

#include "importwizard.h"
#include "threads/remotescannerthread.h"

RemoteImportScanPage::RemoteImportScanPage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(tr("Remote Import Scan"));
    setSubTitle(tr("TODO..."));

    auto layout = new QVBoxLayout;

    {
        auto hboxLayout = new QHBoxLayout;

        m_labelAnimation = new QLabel;
        hboxLayout->addWidget(m_labelAnimation);

        m_labelStatus = new QLabel;
        hboxLayout->addWidget(m_labelStatus, 1);

        layout->addLayout(hboxLayout);
    }

    m_logView = new QPlainTextEdit;
    m_logView->setReadOnly(true);

    layout->addWidget(m_logView, 1);

    setLayout(layout);
}

RemoteImportScanPage::~RemoteImportScanPage() = default;

int RemoteImportScanPage::nextId() const
{
    return int(ImportWizard::Pages::RemoteImportOverview);
}

void RemoteImportScanPage::initializePage()
{
    wizard()->setProperty("result", QVariant::fromValue(ScanResult()));

    m_labelAnimation->setMovie(&m_movieLoading);
    m_movieLoading.start();

    m_logView->clear();

    m_thread = std::make_unique<RemoteScannerThread>(field("folder").toString(), this);

    connect(m_thread.get(), &RemoteScannerThread::progressUpdate, this, &RemoteImportScanPage::progressUpdate);
    connect(m_thread.get(), &RemoteScannerThread::logMessage, this, &RemoteImportScanPage::logMessage);
    connect(m_thread.get(), &RemoteScannerThread::finished, this, &RemoteImportScanPage::finished);

    m_thread->start();
}

void RemoteImportScanPage::cleanupPage()
{
    if (m_thread)
    {
        m_thread->requestInterruption();
        m_thread->wait();
        m_thread = nullptr;
    }
    m_movieLoading.stop();
}

bool RemoteImportScanPage::isComplete() const
{
    return m_thread == nullptr && !scanResultEmpty(wizard()->property("result").value<ScanResult>());
}

void RemoteImportScanPage::progressUpdate(int totalFiles, int skippedFiles)
{
    m_labelStatus->setText(tr("%0 files scanned... (%1 files skipped)").arg(totalFiles).arg(skippedFiles));
}

void RemoteImportScanPage::logMessage(const QString &message)
{
    m_logView->appendHtml(QString("%0: %1<br/>").arg(QTime::currentTime().toString(), message));
}

void RemoteImportScanPage::finished(const ScanResult &result)
{
    m_labelAnimation->setMovie(nullptr);

    const auto success = !scanResultEmpty(result);
    if (success)
    {
        logMessage(tr("Finished"));
        m_labelAnimation->setPixmap(m_pixmapSucceeded);

        wizard()->setProperty("result", QVariant::fromValue(result));
    }
    else
    {
        logMessage(tr("Scan failed."));
        m_labelAnimation->setPixmap(m_pixmapFailed);
    }

    cleanupPage();
    if (success)
        emit completeChanged();
}
