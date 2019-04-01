#include "remoteimportoverviewpage.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateEdit>
#include <QComboBox>
#include <QListView>
#include <QMessageBox>

#include <QDir>
#include <QDirIterator>
#include <QVariant>
#include <QRegularExpression>
#include <QSet>
#include <QDate>
#include <QStringBuilder>

#include "importwizard.h"
#include "common.h"

RemoteImportOverviewPage::RemoteImportOverviewPage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(tr("Remote Import Overview"));
    setSubTitle(tr("TODO...."));
    setCommitPage(true);

    connect(&m_modelHosts, &ChecklistModel::dataChanged, this, &RemoteImportOverviewPage::updateSummary);
    connect(&m_modelProcesses, &ChecklistModel::dataChanged, this, &RemoteImportOverviewPage::updateSummary);

    auto layout = new QVBoxLayout;

    {
        auto groupBox = new QGroupBox(tr("Date-Range:"));

        auto hboxLayout = new QHBoxLayout;

        {
            auto label = new QLabel(tr("From:"));
            hboxLayout->addWidget(label);
        }

        m_dateEditFrom = new QDateEdit;
        connect(m_dateEditFrom, &QDateTimeEdit::dateChanged, this, &RemoteImportOverviewPage::updateSummary);
        hboxLayout->addWidget(m_dateEditFrom);

        hboxLayout->addStretch(1);

        {
            auto label = new QLabel(tr("To:"));
            hboxLayout->addWidget(label);
        }

        m_dateEditTo = new QDateEdit;
        connect(m_dateEditTo, &QDateTimeEdit::dateChanged, this, &RemoteImportOverviewPage::updateSummary);
        hboxLayout->addWidget(m_dateEditTo);

        hboxLayout->addStretch(1);

        {
            auto label = new QLabel(tr("Timestamp:"));
            hboxLayout->addWidget(label);
        }

        m_comboBox = new QComboBox;
        m_comboBox->addItem(tr("Without milliseconds"), "HH:mm:ss");
        m_comboBox->addItem(tr("With milliseconds"), "HH:mm:ss.zzz");
        hboxLayout->addWidget(m_comboBox);

        groupBox->setLayout(hboxLayout);

        layout->addWidget(groupBox);
    }

    {
        auto hboxLayout = new QHBoxLayout;

        {
            auto vboxLayout = new QVBoxLayout;

            {
                auto label = new QLabel(tr("Hosts:"));
                vboxLayout->addWidget(label);
            }

            {
                auto view = new QListView;
                view->setModel(&m_modelHosts);
                vboxLayout->addWidget(view, 1);
            }

            hboxLayout->addLayout(vboxLayout);
        }

        {
            auto vboxLayout = new QVBoxLayout;

            {
                auto label = new QLabel(tr("Processes:"));
                vboxLayout->addWidget(label);
            }

            {
                auto view = new QListView;
                view->setModel(&m_modelProcesses);
                vboxLayout->addWidget(view, 1);
            }

            hboxLayout->addLayout(vboxLayout);
        }

        layout->addLayout(hboxLayout, 1);
    }

    m_labelSummary = new QLabel;
    layout->addWidget(m_labelSummary);

    setLayout(layout);
}

void RemoteImportOverviewPage::initializePage()
{
    m_result = wizard()->property("result").value<ScanResult>();

    QDate minDate, maxDate;
    QSet<QString> processes;

    for (auto hostsIter = m_result.constBegin(); hostsIter != m_result.constEnd(); hostsIter++)
        for (auto processesIter = hostsIter.value().constBegin(); processesIter != hostsIter.value().constEnd(); processesIter++)
        {
            processes.insert(processesIter.key());

            for (auto datesIter = processesIter.value().constBegin(); datesIter != processesIter.value().constEnd(); datesIter++)
            {
                if (minDate.isNull() || datesIter.key() < minDate)
                    minDate = datesIter.key();
                if (maxDate.isNull() || datesIter.key() > maxDate)
                    maxDate = datesIter.key();
            }
        }

    m_dateEditFrom->setDate(minDate);
    m_dateEditTo->setDate(maxDate);

    {
        auto hosts = m_result.keys();
        hosts.sort();
        m_modelHosts.setItems(hosts);
    }

    {
        auto processesList = processes.toList();
        processesList.sort();
        m_modelProcesses.setItems(processesList);
    }

    updateSummary();
}

int RemoteImportOverviewPage::nextId() const
{
    return int(ImportWizard::Pages::ImportProgress);
}

bool RemoteImportOverviewPage::validatePage()
{
    const auto result = filterResult(m_result);

    if (scanResultEmpty(result))
    {
        QMessageBox::warning(this, tr("No files to import!"), tr("No files to import!"));
        return false;
    }

    wizard()->setProperty("result", QVariant::fromValue(result));
    wizard()->setProperty("timeFormat", m_comboBox->currentData().toString());

    return true;
}

void RemoteImportOverviewPage::updateSummary()
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

ScanResult RemoteImportOverviewPage::filterResult(ScanResult result) const
{
    const auto hosts = m_modelHosts.enabledItems().toSet();
    const auto processes = m_modelProcesses.enabledItems().toSet();

    for (auto hostsIter = result.begin(); hostsIter != result.end(); )
    {
        if (hosts.contains(hostsIter.key()))
        {
            for (auto processesIter = hostsIter.value().begin(); processesIter != hostsIter.value().end(); )
            {
                if (processes.contains(processesIter.key()))
                {
                    for (auto datesIter = processesIter.value().begin(); datesIter != processesIter.value().end(); )
                    {
                        if (datesIter.key() >= m_dateEditFrom->date() && datesIter.key() <= m_dateEditTo->date())
                            datesIter++;
                        else
                            datesIter = processesIter.value().erase(datesIter);
                    }

                    processesIter++;
                }
                else
                    processesIter = hostsIter.value().erase(processesIter);
            }

            hostsIter++;
        }
        else
            hostsIter = result.erase(hostsIter);
    }

    return result;
}
