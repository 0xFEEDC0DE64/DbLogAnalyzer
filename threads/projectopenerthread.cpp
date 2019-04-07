#include "projectopenerthread.h"

#include "common.h"

ProjectOpenerThread::ProjectOpenerThread(QSqlDatabase &database, Project &project, QObject *parent) :
    QThread(parent),
    m_database(database),
    m_project(project)
{
}

void ProjectOpenerThread::run()
{

}
