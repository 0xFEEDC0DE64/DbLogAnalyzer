#include "intropage.h"
#include "ui_intropage.h"

#include "importwizard.h"

IntroPage::IntroPage(QWidget *parent) :
    QWizardPage(parent),
    m_ui(std::make_unique<Ui::IntroPage>())
{
    m_ui->setupUi(this);
}

IntroPage::~IntroPage() = default;

int IntroPage::nextId() const
{
    return int(ImportWizard::Pages::Database);
}
