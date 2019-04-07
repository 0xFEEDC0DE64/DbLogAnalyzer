#include "databasewidget.h"
#include "ui_databasewidget.h"

DatabaseWidget::DatabaseWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_unique<Ui::DatabaseWidget>())
{
    m_ui->setupUi(this);

    m_ui->comboBox->addItem(tr("SQLite"), "QSQLITE");
    m_ui->comboBox->addItem(tr("MySQL"), "QMYSQL");

    // for debugging
    setDriver("QMYSQL");
    setMysqlHostname("sql7.freemysqlhosting.net");
    setMysqlUsername("sql7285815");
    setMysqlPassword("BKhysrtqKl");
    setMysqlDatabase("sql7285815");

//    setMysqlHostname("brunner.ninja");

//    setMysqlHostname("localhost");
//    setMysqlUsername("logtest");
//    setMysqlPassword("logtest");
//    setMysqlDatabase("logtest");
}

DatabaseWidget::~DatabaseWidget() = default;

QString DatabaseWidget::driver() const
{
    return m_ui->comboBox->currentData().toString();
}

void DatabaseWidget::setDriver(const QString &driver)
{
    m_ui->comboBox->setCurrentIndex(m_ui->comboBox->findData(driver));
}

QString DatabaseWidget::sqliteFilepath() const
{
    return m_ui->fileSelectionWidget->path();
}

void DatabaseWidget::setSqliteFilepath(const QString &sqliteFilepath)
{
    m_ui->fileSelectionWidget->setPath(sqliteFilepath);
}

QString DatabaseWidget::mysqlHostname() const
{
    return m_ui->lineEditHostname->text();
}

void DatabaseWidget::setMysqlHostname(const QString &mysqlHostname)
{
    m_ui->lineEditHostname->setText(mysqlHostname);
}

QString DatabaseWidget::mysqlUsername() const
{
    return m_ui->lineEditUsername->text();
}

void DatabaseWidget::setMysqlUsername(const QString &mysqlUsername)
{
    m_ui->lineEditUsername->setText(mysqlUsername);
}

QString DatabaseWidget::mysqlPassword() const
{
    return m_ui->lineEditPassword->text();
}

void DatabaseWidget::setMysqlPassword(const QString &mysqlPassword)
{
    m_ui->lineEditPassword->setText(mysqlPassword);
}

QString DatabaseWidget::mysqlDatabase() const
{
    return m_ui->lineEditDatabase->text();
}

void DatabaseWidget::setMysqlDatabase(const QString &mysqlDatabase)
{
    m_ui->lineEditDatabase->setText(mysqlDatabase);
}

QSqlDatabase DatabaseWidget::createConnection(const QString &connectionName)
{
    auto db = QSqlDatabase::addDatabase(driver(), connectionName);

    if (db.driverName() == "QSQLITE")
        db.setDatabaseName(sqliteFilepath());
    else
    {
        db.setHostName(mysqlHostname());
        db.setUserName(mysqlUsername());
        db.setPassword(mysqlPassword());
        db.setDatabaseName(mysqlDatabase());
    }

    return db;
}
