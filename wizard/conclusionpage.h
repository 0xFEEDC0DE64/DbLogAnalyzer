#pragma once

#include <QWizardPage>

class QLabel;
class QCheckBox;

class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit ConclusionPage(QWidget *parent = nullptr);

    void initializePage() override;

private:
    QLabel *m_label;
    QCheckBox *m_checkBox;
};
