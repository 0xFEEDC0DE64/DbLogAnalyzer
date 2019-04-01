#include "conclusionpage.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QStringBuilder>
#include <QVariant>

#include "importwizard.h"

ConclusionPage::ConclusionPage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(tr("Conclusion"));
    setSubTitle(tr("Import successfully finished!"));

    auto layout = new QVBoxLayout;

    m_label = new QLabel;
    layout->addWidget(m_label);

    layout->addStretch(1);

    m_checkBox = new QCheckBox(tr("Open new database"));
    m_checkBox->setChecked(true);
    layout->addWidget(m_checkBox);
    registerField("open", m_checkBox);

    layout->addStretch(1);

    setLayout(layout);
}

void ConclusionPage::initializePage()
{
    auto importWizard = qobject_cast<ImportWizard*>(wizard());
    Q_ASSERT(importWizard);
    Q_ASSERT(importWizard->database().isOpen());

    QSqlQuery query("SELECT COUNT(*) FROM `Logs`;", importWizard->database());
    if (query.lastError().isValid())
    {
        QMessageBox::warning(nullptr, tr("Could not get count!"), tr("Could not get count!") % "\n\n" % query.lastError().text());
        return;
    }

    const auto fetched = query.next();
    Q_ASSERT(fetched);

    m_label->setText(tr("<b>%0 rows</b> have been imported.").arg(query.value(0).toInt()));
}
