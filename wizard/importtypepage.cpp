#include "importtypepage.h"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>
#include <QDir>
#include <QMessageBox>
#include <QSet>
#include <QFileDialog>

#include "importwizard.h"
#include "common.h"

ImportTypePage::ImportTypePage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(tr("Import type"));
    setSubTitle(tr("Please select which type of log files you would like to import."));

    auto layout = new QVBoxLayout;

    m_radioLocal = new QRadioButton(tr("Local: Typically found under /tmp/testfw_log/tests"));
    m_radioLocal->setChecked(true);
    layout->addWidget(m_radioLocal);

    m_radioRemote = new QRadioButton(tr("Remote: Typically found under /log or /log2"));
    layout->addWidget(m_radioRemote);

    layout->addStretch(1);

    {
        auto hboxLayout = new QHBoxLayout;

        m_lineEdit = new QLineEdit;
        hboxLayout->addWidget(m_lineEdit, 1);
        registerField("folder", m_lineEdit);

        {
            auto toolButton = new QToolButton;
            toolButton->setText(tr("Select..."));
            connect(toolButton, &QAbstractButton::pressed, this, &ImportTypePage::selectFolder);
            hboxLayout->addWidget(toolButton);
        }

        layout->addLayout(hboxLayout);
    }

    layout->addStretch(1);

    setLayout(layout);
}

int ImportTypePage::nextId() const
{
    if (m_radioLocal->isChecked())
        return int(ImportWizard::Pages::LocalImport);
    if (m_radioRemote->isChecked())
        return int(ImportWizard::Pages::RemoteImportScan);
    Q_UNREACHABLE();
}

bool ImportTypePage::validatePage()
{
    if (m_lineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("No logfolder defined!"), tr("No logfolder defined!"));
        return false;
    }

    QDir dir(m_lineEdit->text());
    if (!dir.exists())
    {
        QMessageBox::warning(this, tr("Could not find logfolder!"), tr("Could not find logfolder!"));
        return false;
    }

    if (m_radioLocal->isChecked())
    {
        ScanResult result;
        auto &host = result["__dummyHost"];

        for (const auto &fileInfo : dir.entryInfoList({ "*.log" }, QDir::Files))
        {
            if (fileInfo.baseName().endsWith("_console"))
                continue;

            host[fileInfo.baseName()][QDate()] = {
                fileInfo.fileName(),
                fileInfo.absoluteFilePath(),
                fileInfo.size(),
                false
            };
        }

        if (host.isEmpty())
        {
            QMessageBox::warning(this, tr("Could not find any logs!"), tr("Could not find any logs!"));
            return false;
        }

        wizard()->setProperty("result", QVariant::fromValue(result));
    }

    if (m_radioRemote->isChecked())
        wizard()->setProperty("folder", dir.absolutePath());

    return true;
}

void ImportTypePage::selectFolder()
{
    const auto path = QFileDialog::getExistingDirectory(this, tr("Select log folder"));
    if (!path.isEmpty())
        m_lineEdit->setText(path);
}
