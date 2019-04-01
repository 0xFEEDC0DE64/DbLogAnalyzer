#include "databasepage.h"
#include "ui_databasepage.h"

#include <QVBoxLayout>
#include <QSqlError>
#include <QMessageBox>
#include <QStringBuilder>
#include <QSqlQuery>
#include <QFileDialog>

#include "widgets/databasewidget.h"
#include "importwizard.h"

DatabasePage::DatabasePage(QWidget *parent) :
    QWizardPage(parent),
    m_ui(std::make_unique<Ui::DatabasePage>())
{
    m_ui->setupUi(this);

    setCommitPage(true);
}

DatabasePage::~DatabasePage() = default;

int DatabasePage::nextId() const
{
    return int(ImportWizard::Pages::Tables);
}

bool DatabasePage::validatePage()
{
    auto importWizard = qobject_cast<ImportWizard*>(wizard());
    Q_ASSERT(importWizard);
    Q_ASSERT(!importWizard->database().isOpen());

    importWizard->database() = m_ui->databaseWidget->createConnection();

    if (!importWizard->database().open())
    {
        QMessageBox::warning(this, tr("Could not open database!"), tr("Could not open database!") % "\n\n" % importWizard->database().lastError().text());
        importWizard->database() = {};
        return false;
    }

    return true;
}
