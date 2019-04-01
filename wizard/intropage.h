#pragma once

#include <QWizardPage>

#include <memory>

namespace Ui { class IntroPage; }

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit IntroPage(QWidget *parent = nullptr);
    ~IntroPage() override;

    int nextId() const override;

private:
    const std::unique_ptr<Ui::IntroPage> m_ui;
};
