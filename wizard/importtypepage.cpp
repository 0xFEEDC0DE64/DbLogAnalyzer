#include "importtypepage.h"
#include "ui_importtypepage.h"

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
    QWizardPage(parent),
    m_ui(std::make_unique<Ui::ImportTypePage>())
{
    m_ui->setupUi(this);
    m_ui->fileSelectionWidget->setMode(FileSelectionWidget::Mode::ExistingDirectory);
}

ImportTypePage::~ImportTypePage() = default;

int ImportTypePage::nextId() const
{
    if (m_ui->radioButtonLocal->isChecked())
        return int(ImportWizard::Pages::LocalImport);
    if (m_ui->radioButtonRemote->isChecked())
        return int(ImportWizard::Pages::RemoteImportScan);
    Q_UNREACHABLE();
}

bool ImportTypePage::validatePage()
{
    if (m_ui->fileSelectionWidget->path().isEmpty())
    {
        QMessageBox::warning(this, tr("No logfolder defined!"), tr("No logfolder defined!"));
        return false;
    }

    QDir dir(m_ui->fileSelectionWidget->path());
    if (!dir.exists())
    {
        QMessageBox::warning(this, tr("Could not find logfolder!"), tr("Could not find logfolder!"));
        return false;
    }

    if (m_ui->radioButtonLocal->isChecked())
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

    if (m_ui->radioButtonRemote->isChecked())
        wizard()->setProperty("folder", dir.absolutePath());

    return true;
}
