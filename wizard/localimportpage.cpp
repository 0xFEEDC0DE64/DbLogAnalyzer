#include "localimportpage.h"
#include "ui_localimportpage.h"

#include <QHostInfo>
#include <QDate>
#include <QMessageBox>
#include <QStringBuilder>

#include "importwizard.h"

LocalImportPage::LocalImportPage(QWidget *parent) :
    QWizardPage(parent),
    m_ui(std::make_unique<Ui::LocalImportPage>())
{
    m_ui->setupUi(this);

    setCommitPage(true);

    m_ui->lineEditHost->setText(QHostInfo::localHostName());
    m_ui->dateEdit->setDate(QDate::currentDate());
    m_ui->comboBoxTimestamp->addItem(tr("Without milliseconds"), "HH:mm:ss");
    m_ui->comboBoxTimestamp->addItem(tr("With milliseconds"), "HH:mm:ss.zzz");

    m_ui->listView->setModel(&m_model);

    connect(&m_model, &ChecklistModel::dataChanged, this, &LocalImportPage::updateSummary);
}

LocalImportPage::~LocalImportPage() = default;

int LocalImportPage::nextId() const
{
    return int(ImportWizard::Pages::ImportProgress);
}

void LocalImportPage::initializePage()
{
    m_result = wizard()->property("result").value<ScanResult>();

    Q_ASSERT(m_result.count() == 1);

    {
        auto processes = m_result.values().first().keys();
        processes.sort();
        m_model.setItems(processes);
    }

    updateSummary();
}

bool LocalImportPage::validatePage()
{
    auto result = filterResult(m_result);

    if (scanResultEmpty(result))
    {
        QMessageBox::warning(this, tr("No files to import!"), tr("No files to import!"));
        return false;
    }

    Q_ASSERT(result.count() == 1);

    auto host = result.values().first();
    for (auto iter = host.begin(); iter != host.end(); iter++)
    {
        auto &dates = iter.value();
        Q_ASSERT(dates.count() == 1);

        const auto logfile = dates.values().first();
        dates.clear();
        dates.insert(m_ui->dateEdit->date(), logfile);
    }

    result.clear();
    result.insert(m_ui->lineEditHost->text(), host);

    wizard()->setProperty("result", QVariant::fromValue(result));
    wizard()->setProperty("timeFormat", m_ui->comboBoxTimestamp->currentData().toString());

    return true;
}

void LocalImportPage::updateSummary()
{
    if (m_result.isEmpty())
        return;

    const auto result = filterResult(m_result);

    int logFiles { 0 };
    qint64 totalSize { 0 };

    for (auto hostsIter = result.constBegin(); hostsIter != result.constEnd(); hostsIter++)
        for (auto processesIter = hostsIter.value().constBegin(); processesIter != hostsIter.value().constEnd(); processesIter++)
            for (auto datesIter = processesIter.value().constBegin(); datesIter != processesIter.value().constEnd(); datesIter++)
            {
                logFiles++;
                totalSize += datesIter.value().filesize;
            }

    QString sizeStr;
    for (const QString prefix : { "K", "M", "G", "T" })
    {
        if (totalSize > 1024)
        {
            totalSize /= 1024;
            sizeStr = QString::number(totalSize) % prefix;
        }
    }

    m_ui->labelSummary->setText(tr("Filters match %0 files (%1B)").arg(logFiles).arg(sizeStr));
}

ScanResult LocalImportPage::filterResult(ScanResult result) const
{
    const auto processes = m_model.enabledTexts().toSet();

    for (auto hostsIter = result.begin(); hostsIter != result.end(); hostsIter++)
    {
        for (auto processesIter = hostsIter.value().begin(); processesIter != hostsIter.value().end(); )
        {
            if (processes.contains(processesIter.key()))
                processesIter++;
            else
                processesIter = hostsIter.value().erase(processesIter);
        }
    }

    return result;
}
