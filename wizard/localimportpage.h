#pragma once

#include <QWizardPage>

#include <memory>

#include "models/checklistmodel.h"
#include "common.h"

namespace Ui { class LocalImportPage; }

class LocalImportPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit LocalImportPage(QWidget *parent = nullptr);
    ~LocalImportPage() override;

    int nextId() const override;

    void initializePage() override;
    bool validatePage() override;

private slots:
    void updateSummary();

private:
    const std::unique_ptr<Ui::LocalImportPage> m_ui;

    ScanResult filterResult(ScanResult result) const;

    ScanResult m_result;

    ChecklistModel m_model;
};
