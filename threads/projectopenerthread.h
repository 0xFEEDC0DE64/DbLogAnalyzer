#pragma once

#include <QThread>

class QSqlDatabase;

class Project;

class ProjectOpenerThread : public QThread
{
    Q_OBJECT

public:
    ProjectOpenerThread(QSqlDatabase &database, Project &project, QObject *parent = nullptr);

protected:
    void run() override;

private:
    QSqlDatabase &m_database;
    Project &m_project;
};
