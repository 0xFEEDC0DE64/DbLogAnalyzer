#include "opendialog.h"
#include "ui_opendialog.h"

#include <QMessageBox>
#include <QStringBuilder>
#include <QSqlError>

OpenDialog::OpenDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_unique<Ui::OpenDialog>())
{
    m_ui->setupUi(this);

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &OpenDialog::submit);
}

OpenDialog::~OpenDialog() = default;

QSqlDatabase OpenDialog::database()
{
    return m_database;
}

void OpenDialog::submit()
{
    m_database = m_ui->databaseWidget->createConnection();

    if (!m_database.open())
    {
        QMessageBox::warning(this, tr("Could not open database!"), tr("Could not open database!") % "\n\n" % m_database.lastError().text());
        m_database = {};
        return;
    }

    accept();
}
