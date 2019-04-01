#pragma once

#include <QWizardPage>

class QRadioButton;
class QLineEdit;

class ImportTypePage : public QWizardPage
{
    Q_OBJECT

public:
    explicit ImportTypePage(QWidget *parent = nullptr);

    int nextId() const override;

    bool validatePage() override;

private slots:
    void selectFolder();

private:
    QRadioButton *m_radioLocal;
    QRadioButton *m_radioRemote;

    QLineEdit *m_lineEdit;
};
