#pragma once

#include <QWizardPage>

#include <memory>

#include "models/progressmodel.h"

class QLabel;
class QSqlDatabase;

class TableCreatorThread;
namespace Ui { class TablesPage; }

class TablesPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit TablesPage(QWidget *parent = nullptr);
    ~TablesPage() override;

    int nextId() const override;

    void initializePage() override;
    void cleanupPage() override;
    bool isComplete() const override;

private slots:
    void someSlot(int index);

private:
    void stopThread();

    const std::unique_ptr<Ui::TablesPage> m_ui;

    ProgressModel m_model;

    std::unique_ptr<TableCreatorThread> m_thread;
};
