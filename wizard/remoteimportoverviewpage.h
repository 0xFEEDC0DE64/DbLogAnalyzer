#pragma once

#include <QWizardPage>

#include "models/checklistmodel.h"
#include "common.h"

class QDateEdit;
class QComboBox;
class QLabel;

class RemoteImportOverviewPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit RemoteImportOverviewPage(QWidget *parent = nullptr);

    void initializePage() override;

    int nextId() const override;

    bool validatePage() override;

private slots:
    void updateSummary();

private:
    ScanResult filterResult(ScanResult result) const;

    QDateEdit *m_dateEditFrom;
    QDateEdit *m_dateEditTo;
    QComboBox *m_comboBox;
    QLabel *m_labelSummary;

    ChecklistModel m_modelHosts;
    ChecklistModel m_modelProcesses;

    ScanResult m_result;
};
