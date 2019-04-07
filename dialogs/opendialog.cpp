#include "opendialog.h"
#include "ui_opendialog.h"

#include <QMessageBox>
#include <QStringBuilder>
#include <QSqlError>
#include <QSqlQuery>

OpenDialog::OpenDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_unique<Ui::OpenDialog>())
{
    m_ui->setupUi(this);

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &OpenDialog::submit);
}

OpenDialog::~OpenDialog() = default;

std::unique_ptr<Project> &OpenDialog::project()
{
    return m_project;
}

void OpenDialog::submit()
{
    auto project = std::make_unique<Project>();
    project->database = m_ui->databaseWidget->createConnection();

    if (!project->database.open())
    {
        QMessageBox::warning(this, tr("Could not open database!"), tr("Could not open database!") % "\n\n" % project->database.lastError().text());
        return;
    }

    {
        QSqlQuery query("SELECT `ID`, `Name` FROM `Hosts`;", project->database);
        if (query.lastError().isValid())
        {
            QMessageBox::warning(this, tr("Could not open database!"), tr("Could not open database!") % "\n\n" % query.lastError().text());
            return;
        }
        while(query.next())
            project->hosts.insert(query.value(0).toInt(), query.value(1).toString());
    }

    {
        QSqlQuery query("SELECT `ID`, `Name` FROM `Processes`;", project->database);
        if (query.lastError().isValid())
        {
            QMessageBox::warning(this, tr("Could not open database!"), tr("Could not open database!") % "\n\n" % query.lastError().text());
            return;
        }
        while(query.next())
            project->processes.insert(query.value(0).toInt(), query.value(1).toString());
    }

    {
        QSqlQuery query("SELECT `ID`, `Name` FROM `Filenames`;", project->database);
        if (query.lastError().isValid())
        {
            QMessageBox::warning(this, tr("Could not open database!"), tr("Could not open database!") % "\n\n" % query.lastError().text());
            return;
        }
        while(query.next())
            project->filenames.insert(query.value(0).toInt(), query.value(1).toString());
    }

    {
        QSqlQuery query("SELECT `ID`, `Name` FROM `Threads`;", project->database);
        if (query.lastError().isValid())
        {
            QMessageBox::warning(this, tr("Could not open database!"), tr("Could not open database!") % "\n\n" % query.lastError().text());
            return;
        }
        while(query.next())
            project->threads.insert(query.value(0).toInt(), query.value(1).toString());
    }

    {
        QSqlQuery query("SELECT `ID`, `Name` FROM `Types`;", project->database);
        if (query.lastError().isValid())
        {
            QMessageBox::warning(this, tr("Could not open database!"), tr("Could not open database!") % "\n\n" % query.lastError().text());
            return;
        }
        while(query.next())
            project->types.insert(query.value(0).toInt(), query.value(1).toString());
    }

    m_project = std::move(project);

    accept();
}
