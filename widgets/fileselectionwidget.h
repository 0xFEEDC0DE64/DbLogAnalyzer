#pragma once

#include <QWidget>

#include <memory>

namespace Ui { class FileSelectionWidget; }

class FileSelectionWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged USER true)

public:
    enum class Mode {
        OpenFile, SaveFile, ExistingDirectory
    };

    explicit FileSelectionWidget(QWidget *parent = nullptr);
    FileSelectionWidget(const Mode mode, QWidget *parent = nullptr);
    FileSelectionWidget(const Mode mode, const QString &path, QWidget *parent = nullptr);
    ~FileSelectionWidget() override;

    Mode mode() const;
    void setMode(const Mode mode);

    QString path() const;
    void setPath(const QString &path);

signals:
    void pathChanged(const QString &path);

private slots:
    void selectPath();

private:
    const std::unique_ptr<Ui::FileSelectionWidget> m_ui;
    Mode m_mode;
};
