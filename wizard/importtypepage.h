#pragma once

#include <QWizardPage>

#include <memory>

namespace Ui { class ImportTypePage; }

class ImportTypePage : public QWizardPage
{
    Q_OBJECT

public:
    explicit ImportTypePage(QWidget *parent = nullptr);
    ~ImportTypePage() override;

    int nextId() const override;

    bool validatePage() override;

private:
    const std::unique_ptr<Ui::ImportTypePage> m_ui;
};
