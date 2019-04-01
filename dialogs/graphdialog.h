#pragma once

#include <QDialog>

#include <memory>

class QSqlDatabase;

namespace Ui { class GraphDialog; }

class GraphDialog : public QDialog
{
    Q_OBJECT

public:
    GraphDialog(QSqlDatabase &database, QWidget *parent = nullptr);
    ~GraphDialog() override;

private:
    const std::unique_ptr<Ui::GraphDialog> m_ui;

    QSqlDatabase &m_database;
};
