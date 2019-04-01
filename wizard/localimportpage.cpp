#include "localimportpage.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QHostInfo>
#include <QDateEdit>
#include <QComboBox>
#include <QListView>
#include <QLabel>
#include <QMessageBox>
#include <QStringBuilder>

#include "importwizard.h"

LocalImportPage::LocalImportPage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(tr("Local Import"));
    setSubTitle(tr("TODO..."));
    setCommitPage(true);

    connect(&m_model, &ChecklistModel::dataChanged, this, &LocalImportPage::updateSummary);

    auto layout = new QVBoxLayout;

    {
        auto hboxLayout = new QHBoxLayout;

        {
            auto formLayout = new QFormLayout;

            m_lineEditHost = new QLineEdit(QHostInfo::localHostName());
            formLayout->addRow(tr("Host:"), m_lineEditHost);

            m_dateEdit = new QDateEdit(QDate::currentDate());
            formLayout->addRow(tr("Date:"), m_dateEdit);

            m_comboBox = new QComboBox;
            m_comboBox->addItem(tr("Without milliseconds"), "HH:mm:ss");
            m_comboBox->addItem(tr("With milliseconds"), "HH:mm:ss.zzz");
            formLayout->addRow(tr("Timestamp:"), m_comboBox);

            hboxLayout->addLayout(formLayout);
        }

        {
            auto view = new QListView;
            view->setModel(&m_model);
            hboxLayout->addWidget(view, 1);
        }

        layout->addLayout(hboxLayout, 1);
    }

    m_labelSummary = new QLabel;
    layout->addWidget(m_labelSummary);

    setLayout(layout);
}

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
        dates.insert(m_dateEdit->date(), logfile);
    }

    result.clear();
    result.insert(m_lineEditHost->text(), host);

    wizard()->setProperty("result", QVariant::fromValue(result));
    wizard()->setProperty("timeFormat", m_comboBox->currentData().toString());

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

    m_labelSummary->setText(tr("Filters match %0 files (%1B)").arg(logFiles).arg(sizeStr));
}

ScanResult LocalImportPage::filterResult(ScanResult result) const
{
    const auto processes = m_model.enabledItems().toSet();

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
