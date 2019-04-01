#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSqlRelationalDelegate>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QStringBuilder>
#include <QMenu>
#include <QAction>
#include <QDateTime>
#include <QSqlRecord>
#include <QSqlRelation>
#include <QDebug>

#include "wizard/importwizard.h"
#include "dialogs/opendialog.h"
#include "dialogs/graphdialog.h"
#include "models/sqlrelationaltablemodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(std::make_unique<Ui::MainWindow>())
{
    m_ui->setupUi(this);

    m_ui->actionNew->setShortcut(QKeySequence::New);
    m_ui->actionOpen->setShortcut(QKeySequence::Open);
    m_ui->actionQuit->setShortcut(QKeySequence::Quit);

    connect(m_ui->actionNew, &QAction::triggered, this, &MainWindow::newClicked);
    connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::openClicked);
    connect(m_ui->actionClose, &QAction::triggered, this, &MainWindow::closeClicked);
    connect(m_ui->actionQuit, &QAction::triggered, QCoreApplication::instance(), &QCoreApplication::quit);
    connect(m_ui->actionGraph, &QAction::triggered, this, &MainWindow::graphClicked);

    for (QAction *action : { m_ui->actionTimestamp, m_ui->actionHost, m_ui->actionProcess, m_ui->actionFilename, m_ui->actionThread, m_ui->actionType, m_ui->actionMessage })
        connect(action, &QAction::toggled, this, &MainWindow::showColumns);

    connect(m_ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::updateQuery);
    connect(m_ui->pushButton, &QPushButton::pressed, this, &MainWindow::updateQuery);

    m_ui->tableView->setItemDelegate(new QSqlRelationalDelegate(m_ui->tableView));

    connect(m_ui->tableView, &QWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
}

MainWindow::~MainWindow() = default;

void MainWindow::newClicked()
{
    ImportWizard wizard(this);
    if (wizard.exec() == QDialog::Accepted && wizard.field("open").toBool())
    {
        m_ui->actionNew->setVisible(false);
        m_ui->actionOpen->setVisible(false);
        m_ui->actionClose->setVisible(true);
        m_ui->actionGraph->setEnabled(true);
        m_ui->lineEdit->setEnabled(true);
        m_ui->pushButton->setEnabled(true);

        m_database = wizard.database();
        setupModel();
        updateQuery();
    }
}

void MainWindow::openClicked()
{
    OpenDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        m_ui->actionNew->setVisible(false);
        m_ui->actionOpen->setVisible(false);
        m_ui->actionClose->setVisible(true);
        m_ui->actionGraph->setEnabled(true);
        m_ui->lineEdit->setEnabled(true);
        m_ui->pushButton->setEnabled(true);

        m_database = dialog.database();
        setupModel();
        updateQuery();
    }
}

void MainWindow::closeClicked()
{
    m_ui->actionNew->setVisible(true);
    m_ui->actionOpen->setVisible(true);
    m_ui->actionClose->setVisible(false);
    m_ui->actionGraph->setEnabled(false);
    m_ui->lineEdit->setEnabled(false);
    m_ui->pushButton->setEnabled(false);

    m_ui->tableView->setModel(nullptr);
    m_model = nullptr;
    m_database.close();
    m_database = QSqlDatabase();
}

void MainWindow::graphClicked()
{
    GraphDialog(m_database, this).exec();
}

void MainWindow::updateQuery()
{
    auto filter = m_ui->lineEdit->text();
    if (!filter.trimmed().isEmpty())
    {
        filter.replace("||", "OR");
        filter.replace("&&", "AND");
        m_model->setFilter(filter);
    }

    if (!m_model->select())
        QMessageBox::warning(this, tr("Query failed!"), tr("Query failed!") % "\n\n" % m_model->query().lastError().text());
}

void MainWindow::showColumns()
{
    for (const auto &pair : {
        std::make_pair(m_ui->actionTimestamp, ColumnTimestamp),
        std::make_pair(m_ui->actionHost, ColumnHost),
        std::make_pair(m_ui->actionProcess, ColumnProcess),
        std::make_pair(m_ui->actionFilename, ColumnFilename),
        std::make_pair(m_ui->actionThread, ColumnThread),
        std::make_pair(m_ui->actionType, ColumnType),
        std::make_pair(m_ui->actionMessage, ColumnMessage)
    })
        m_ui->tableView->setColumnHidden(std::get<1>(pair), !std::get<0>(pair)->isChecked());
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    const auto index = m_ui->tableView->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu menu(this);
    const auto exec = [this,&menu,&pos](){ return menu.exec(m_ui->tableView->viewport()->mapToGlobal(pos)); };

    qDebug() << m_model->record(index.row()).value(2);
    const auto data = m_model->data(index, Qt::EditRole).toString();

    switch (index.column())
    {
    case ColumnTimestamp:
    {
        auto minute = menu.addAction(tr("Filter by minute"));
        auto second = menu.addAction(tr("Filter by second"));
        auto action = exec();
        if (action == minute || action == second)
        {
            const auto format = QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz");

            auto dateTime = QDateTime::fromString(data, format);
            auto time = dateTime.time();
            time.setHMS(time.hour(), time.minute(), action==minute ? 0 : time.second());
            dateTime.setTime(time);
            m_ui->lineEdit->setText(QString("`Timestamp` BETWEEN \"%0\" AND \"%1\"").arg(dateTime.toString(format), dateTime.addMSecs(action==minute ? 59999 : 999).toString(format)));
            updateQuery();
        }
        break;
    }
    case ColumnHost:
        if (menu.addAction(tr("Filter by host")) == exec())
        {
            m_ui->lineEdit->setText(QString("`Hosts`.`Name` = \"%0\"").arg(data));
            updateQuery();
        }
        break;
    case ColumnProcess:
        if (menu.addAction(tr("Filter by process")) == exec())
        {
            m_ui->lineEdit->setText(QString("`Processes`.`Name` = \"%0\"").arg(data));
            updateQuery();
        }
        break;
    case ColumnFilename:
        if (menu.addAction(tr("Filter by filename")) == exec())
        {
            m_ui->lineEdit->setText(QString("`Filenames`.`Name` = \"%0\"").arg(data));
            updateQuery();
        }
        break;
    case ColumnThread:
        if (menu.addAction(tr("Filter by thread")) == exec())
        {
            m_ui->lineEdit->setText(QString("`Threads`.`Name` = \"%0\"").arg(data));
            updateQuery();
        }
        break;
    case ColumnType:
        if (menu.addAction(tr("Filter by type")) == exec())
        {
            m_ui->lineEdit->setText(QString("`Types`.`Name` = \"%0\"").arg(data));
            updateQuery();
        }
        break;
    }
}

void MainWindow::setupModel()
{
    m_ui->tableView->setModel(nullptr);
    m_model = std::make_unique<SqlRelationalTableModel>(this, m_database);
    m_model->setTable("Logs");
    m_model->setRelation(ColumnHost, QSqlRelation("Hosts", "ID", "Name"));
    m_model->setRelation(ColumnProcess, QSqlRelation("Processes", "ID", "Name"));
    m_model->setRelation(ColumnFilename, QSqlRelation("Filenames", "ID", "Name"));
    m_model->setRelation(ColumnThread, QSqlRelation("Threads", "ID", "Name"));
    m_model->setRelation(ColumnType, QSqlRelation("Types", "ID", "Name"));
    m_ui->tableView->setModel(m_model.get());
    m_ui->tableView->setColumnHidden(ColumnID, true);
    showColumns();
}
