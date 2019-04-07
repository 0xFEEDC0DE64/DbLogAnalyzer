#include "tablespage.h"
#include "ui_tablespage.h"

#include "importwizard.h"
#include "threads/tablecreatorthread.h"

TablesPage::TablesPage(QWidget *parent) :
    QWizardPage(parent),
    m_ui(std::make_unique<Ui::TablesPage>()),
    m_model(this)
{
    m_ui->setupUi(this);

    m_ui->listView->setModel(&m_model);
}

TablesPage::~TablesPage() = default;

int TablesPage::nextId() const
{
    return int(ImportWizard::Pages::ImportType);
}

void TablesPage::initializePage()
{
    auto importWizard = qobject_cast<ImportWizard*>(wizard());
    Q_ASSERT(importWizard);
    Q_ASSERT(importWizard->database().isOpen());

    QVector<ProgressModel::Item> items;
    items.reserve(TableCreatorThread::tables().size());

    for (const QString &tableName : TableCreatorThread::tables())
        items.append({ tr("Create table %0").arg(tableName), ProgressModel::Item::Status::None });
    items.first().status = ProgressModel::Item::Status::Loading;

    m_model.setItems(items);

    m_thread = std::make_unique<TableCreatorThread>(importWizard->database(), this);
    connect(m_thread.get(), &TableCreatorThread::someSignal, this, &TablesPage::someSlot);

    m_thread->start();
}

void TablesPage::cleanupPage()
{
    stopThread();
    m_model.clearItems();
}

bool TablesPage::isComplete() const
{
    return m_thread == nullptr;
}

void TablesPage::someSlot(int index)
{
    Q_ASSERT(index < m_model.rowCount({}));

    m_model.setStatus(index, ProgressModel::Item::Status::Succeeded);
    if (index < m_model.rowCount({}) - 1)
        m_model.setStatus(index + 1, ProgressModel::Item::Status::Loading);
    else
    {
        stopThread();
        emit completeChanged();
    }
}

void TablesPage::stopThread()
{
    if (m_thread)
    {
        m_thread->requestInterruption();
        m_thread->wait();
        m_thread = nullptr;
    }
}
