#include "importprogresspage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPlainTextEdit>

#include "importwizard.h"
#include "threads/importthread.h"

ImportProgressPage::ImportProgressPage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(tr("Import Progress"));
    setSubTitle(tr("TODO..."));

    auto layout = new QVBoxLayout;

    {
        auto hboxLayout = new QHBoxLayout;

        m_labelIcon = new QLabel;
        hboxLayout->addWidget(m_labelIcon);

        m_labelStatus = new QLabel;
        hboxLayout->addWidget(m_labelStatus, 1);

        layout->addLayout(hboxLayout);
    }

    m_progressBar = new QProgressBar;
    m_progressBar->setMaximum(100);
    layout->addWidget(m_progressBar);

    m_logView = new QPlainTextEdit;
    m_logView->setReadOnly(true);
    layout->addWidget(m_logView, 1);

    setLayout(layout);
}

ImportProgressPage::~ImportProgressPage() = default;

void ImportProgressPage::initializePage()
{
    auto importWizard = qobject_cast<ImportWizard*>(wizard());
    Q_ASSERT(importWizard);
    Q_ASSERT(importWizard->database().isOpen());

    const auto result = wizard()->property("result").value<ScanResult>();
    const auto timeFormat = wizard()->property("timeFormat").toString();

    m_logView->clear();

    m_thread = std::make_unique<ImportThread>(importWizard->database(), result, timeFormat, this);
    connect(m_thread.get(), &ImportThread::statusUpdate, this, &ImportProgressPage::statusUpdate);
    connect(m_thread.get(), &ImportThread::progressUpdate, this, &ImportProgressPage::progressUpdate);
    connect(m_thread.get(), &ImportThread::logMessage, this, &ImportProgressPage::logMessage);
    connect(m_thread.get(), &QThread::finished, this, &ImportProgressPage::finished);
    m_thread->start();

    m_labelIcon->setMovie(&m_movieLoading);
    m_movieLoading.start();
}

void ImportProgressPage::cleanupPage()
{
    if (m_thread)
    {
        m_thread->requestInterruption();
        m_thread->wait();
        m_thread = nullptr;
    }
    m_movieLoading.stop();
}

int ImportProgressPage::nextId() const
{
    return int(ImportWizard::Pages::Conclusion);
}

bool ImportProgressPage::isComplete() const
{
    return m_thread == nullptr;
}

void ImportProgressPage::statusUpdate(const QString &message)
{
    m_labelStatus->setText(message);
}

void ImportProgressPage::progressUpdate(qint64 finished, qint64 total)
{
    while (total & 0xFFFFFFFF00000000)
    {
        finished = finished >> 8;
        total = total >> 8;
    }

    m_progressBar->setMaximum(total);
    m_progressBar->setValue(finished);
}

void ImportProgressPage::logMessage(const QString &message)
{
    m_logView->appendHtml(QString("%0: %1<br/>").arg(QTime::currentTime().toString(), message));
}

void ImportProgressPage::finished()
{
    cleanupPage();
    emit completeChanged();

    m_labelIcon->setPixmap(m_pixmapSucceeded);
    logMessage(tr("Finished."));
}
