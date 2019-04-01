#pragma once

#include <QWidget>
#include <QSqlDatabase>

#include <memory>

namespace Ui { class DatabaseWidget; }

class DatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DatabaseWidget(QWidget *parent = nullptr);
    ~DatabaseWidget() override;

    QString driver() const;
    void setDriver(const QString &driver);

    QString sqliteFilepath() const;
    void setSqliteFilepath(const QString &sqliteFilepath);

    QString mysqlHostname() const;
    void setMysqlHostname(const QString &mysqlHostname);

    QString mysqlUsername() const;
    void setMysqlUsername(const QString &mysqlUsername);

    QString mysqlPassword() const;
    void setMysqlPassword(const QString &mysqlPassword);

    QString mysqlDatabase() const;
    void setMysqlDatabase(const QString &mysqlDatabase);

    QSqlDatabase createConnection(const QString& connectionName = QLatin1String(QSqlDatabase::defaultConnection));

private:
    const std::unique_ptr<Ui::DatabaseWidget> m_ui;
};
