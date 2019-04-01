#pragma once

#include <QDialog>
#include <QSqlDatabase>

#include <memory>

namespace Ui { class OpenDialog; }

class OpenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenDialog(QWidget *parent = nullptr);
    ~OpenDialog() override;

    QSqlDatabase database();

private slots:
    void submit();

private:
    const std::unique_ptr<Ui::OpenDialog> m_ui;

    QSqlDatabase m_database;
};
