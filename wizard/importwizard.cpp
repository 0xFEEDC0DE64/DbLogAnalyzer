#include "importwizard.h"

#include "intropage.h"
#include "databasepage.h"
#include "tablespage.h"
#include "importtypepage.h"
#include "localimportpage.h"
#include "remoteimportscanpage.h"
#include "remoteimportoverviewpage.h"
#include "importprogresspage.h"
#include "conclusionpage.h"

ImportWizard::ImportWizard(QWidget *parent, Qt::WindowFlags flags) :
    QWizard(parent, flags)
{
    setPage(int(Pages::Introduction), new IntroPage);
    setPage(int(Pages::Database), new DatabasePage);
    setPage(int(Pages::Tables), new TablesPage);
    setPage(int(Pages::ImportType), new ImportTypePage);
    setPage(int(Pages::LocalImport), new LocalImportPage);
    setPage(int(Pages::RemoteImportScan), new RemoteImportScanPage);
    setPage(int(Pages::RemoteImportOverview), new RemoteImportOverviewPage);
    setPage(int(Pages::ImportProgress), new ImportProgressPage);
    setPage(int(Pages::Conclusion), new ConclusionPage);
}

QSqlDatabase &ImportWizard::database()
{
    return m_database;
}

const QSqlDatabase &ImportWizard::database() const
{
    return m_database;
}
