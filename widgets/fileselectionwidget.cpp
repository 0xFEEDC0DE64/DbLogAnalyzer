#include "fileselectionwidget.h"
#include "ui_fileselectionwidget.h"

#include <QFileDialog>

FileSelectionWidget::FileSelectionWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_unique<Ui::FileSelectionWidget>()),
    m_mode(Mode::OpenFile)
{
    m_ui->setupUi(this);

    connect(m_ui->lineEdit, &QLineEdit::textChanged, this, &FileSelectionWidget::pathChanged);
    connect(m_ui->pushButton, &QAbstractButton::pressed, this, &FileSelectionWidget::selectPath);
}

FileSelectionWidget::FileSelectionWidget(const Mode mode, QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_unique<Ui::FileSelectionWidget>()),
    m_mode(mode)
{
    m_ui->setupUi(this);

    connect(m_ui->lineEdit, &QLineEdit::textChanged, this, &FileSelectionWidget::pathChanged);
    connect(m_ui->pushButton, &QAbstractButton::pressed, this, &FileSelectionWidget::selectPath);
}

FileSelectionWidget::FileSelectionWidget(const Mode mode, const QString &path, QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_unique<Ui::FileSelectionWidget>()),
    m_mode(mode)
{
    m_ui->setupUi(this);

    m_ui->lineEdit->setText(path);

    connect(m_ui->lineEdit, &QLineEdit::textChanged, this, &FileSelectionWidget::pathChanged);
    connect(m_ui->pushButton, &QAbstractButton::pressed, this, &FileSelectionWidget::selectPath);
}

FileSelectionWidget::~FileSelectionWidget() = default;

FileSelectionWidget::Mode FileSelectionWidget::mode() const
{
    return m_mode;
}

void FileSelectionWidget::setMode(const FileSelectionWidget::Mode mode)
{
    m_mode = mode;
}

QString FileSelectionWidget::path() const
{
    return m_ui->lineEdit->text();
}

void FileSelectionWidget::setPath(const QString &path)
{
    m_ui->lineEdit->setText(path);
}

void FileSelectionWidget::selectPath()
{
    QString path;
    switch (m_mode)
    {
    case Mode::OpenFile:          path = QFileDialog::getOpenFileName(this);      break;
    case Mode::SaveFile:          path = QFileDialog::getSaveFileName(this);      break;
    case Mode::ExistingDirectory: path = QFileDialog::getExistingDirectory(this); break;
    }

    if (!path.isEmpty())
        m_ui->lineEdit->setText(path);
}
