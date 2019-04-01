#pragma once

#include <QWizardPage>

#include <memory>

namespace Ui { class DatabasePage; }

class DatabasePage : public QWizardPage
{
    Q_OBJECT

public:
    explicit DatabasePage(QWidget *parent = nullptr);
    ~DatabasePage() override;

    int nextId() const override;

    bool validatePage() override;

private:
    const std::unique_ptr<Ui::DatabasePage> m_ui;
};
