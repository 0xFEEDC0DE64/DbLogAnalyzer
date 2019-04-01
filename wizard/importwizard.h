#pragma once

#include <QWizard>
#include <QSqlDatabase>

class ImportWizard : public QWizard
{
    Q_OBJECT

public:
    enum class Pages { Introduction, Database, Tables, ImportType, LocalImport, RemoteImportScan, RemoteImportOverview, ImportProgress, Conclusion };

    ImportWizard(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    QSqlDatabase &database();
    const QSqlDatabase &database() const;

private:
    QSqlDatabase m_database;
};
