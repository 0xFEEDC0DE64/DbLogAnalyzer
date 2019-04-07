#pragma once

#include <QDialog>
#include <QSqlDatabase>

#include <memory>

#include "common.h"

namespace Ui { class OpenDialog; }

class OpenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenDialog(QWidget *parent = nullptr);
    ~OpenDialog() override;

    std::unique_ptr<Project> &project();

private slots:
    void submit();

private:
    const std::unique_ptr<Ui::OpenDialog> m_ui;

    std::unique_ptr<Project> m_project;
};
