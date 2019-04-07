#pragma once

#include <QMainWindow>
#include <QSqlDatabase>

#include <memory>

#include "common.h"

class LogModel;

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

    std::unique_ptr<Project> m_project;
    std::unique_ptr<LogModel> m_model;
};
