#pragma once

#include <QWizardPage>
#include <QPixmap>
#include <QMovie>
#include <QVector>

#include <memory>

class QLabel;
class QSqlDatabase;

class TableCreatorThread;

class TablesPage : public QWizardPage
{
    Q_OBJECT

    const QPixmap m_pixmapSucceeded { ":/loganalyzer/succeeded.png" };
    const QPixmap m_pixmapFailed { ":/loganalyzer/failed.png" };
    QMovie m_movieLoading { ":/loganalyzer/loading.gif" };

public:
    explicit TablesPage(QWidget *parent = nullptr);
    ~TablesPage() override;

    int nextId() const override;

    void initializePage() override;
    void cleanupPage() override;
    bool isComplete() const override;

private slots:
    void someSlot(int index);

private:
    QVector<QLabel*> m_statusLabels;

    std::unique_ptr<TableCreatorThread> m_thread;
};
