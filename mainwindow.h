#pragma once

#include <QMainWindow>
#include <QSqlDatabase>

#include <memory>

class QSqlRelationalTableModel;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void newClicked();
    void openClicked();
    void closeClicked();
    void graphClicked();
    void updateQuery();
    void showColumns();
    void showContextMenu(const QPoint &pos);

private:
    void setupModel();

    enum { ColumnID, ColumnTimestamp, ColumnHost, ColumnProcess, ColumnFilename, ColumnThread, ColumnType, ColumnMessage };

    const std::unique_ptr<Ui::MainWindow> m_ui;

    QSqlDatabase m_database;
    std::unique_ptr<QSqlRelationalTableModel> m_model;
};
