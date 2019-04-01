#include "tablespage.h"

#include <QGridLayout>
#include <QLabel>

#include "importwizard.h"
#include "threads/tablecreatorthread.h"

TablesPage::TablesPage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(tr("Tables"));
    setSubTitle(tr("TODO..."));

    auto layout = new QGridLayout;

    m_statusLabels.resize(TableCreatorThread::tables().size());

    int index { 0 };
    for (const QString &tableName : TableCreatorThread::tables())
    {
        m_statusLabels[index] = new QLabel;
        layout->addWidget(m_statusLabels[index], index, 0);

        auto label = new QLabel(tr("Create table %0").arg(tableName));
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->addWidget(label, index++, 1);
    }

    setLayout(layout);
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

    for (auto label : m_statusLabels)
    {
        label->setMovie(nullptr);
        label->setPixmap({});
    }

    m_thread = std::make_unique<TableCreatorThread>(importWizard->database(), this);
    connect(m_thread.get(), &TableCreatorThread::someSignal, this, &TablesPage::someSlot);

    m_thread->start();
    m_statusLabels[0]->setMovie(&m_movieLoading);
    m_movieLoading.start();
}

void TablesPage::cleanupPage()
{
    if (m_thread)
    {
        m_thread->requestInterruption();
        m_thread->wait();
        m_thread = nullptr;
    }
    m_movieLoading.stop();
}

bool TablesPage::isComplete() const
{
    return m_thread == nullptr;
}

void TablesPage::someSlot(int index)
{
    Q_ASSERT(index < m_statusLabels.size());

    m_statusLabels[index]->setMovie(nullptr);
    m_statusLabels[index]->setPixmap(m_pixmapSucceeded);
    if (index < m_statusLabels.size() - 1)
        m_statusLabels[index+1]->setMovie(&m_movieLoading);
    else
    {
        cleanupPage();
        emit completeChanged();
    }
}
