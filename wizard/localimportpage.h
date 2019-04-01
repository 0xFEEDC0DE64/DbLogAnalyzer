#pragma once

#include <QWizardPage>

#include "models/checklistmodel.h"
#include "common.h"

class QLineEdit;
class QDateEdit;
class QComboBox;
class QLabel;

class LocalImportPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit LocalImportPage(QWidget *parent = nullptr);

    int nextId() const override;

    void initializePage() override;
    bool validatePage() override;

private slots:
    void updateSummary();

private:
    ScanResult filterResult(ScanResult result) const;

    ScanResult m_result;

    ChecklistModel m_model;

    QLineEdit *m_lineEditHost;
    QDateEdit *m_dateEdit;
    QComboBox *m_comboBox;
    QLabel *m_labelSummary;
};
