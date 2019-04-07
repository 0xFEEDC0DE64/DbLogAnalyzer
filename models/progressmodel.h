#pragma once

#include <QAbstractListModel>
#include <QPixmap>
#include <QMovie>

class ProgressModel : public QAbstractListModel
{
    Q_OBJECT

public:
    struct Item
    {
        enum class Status
        {
            None,
            Loading,
            Succeeded,
            Failed
        };

        QString text;
        Status status;
    };

    explicit ProgressModel(QObject *parent = nullptr);
    ProgressModel(const QStringList &items, QObject *parent = nullptr);
    ProgressModel(const QVector<Item> &items, QObject *parent = nullptr);

    const QVector<Item> &items() const;
    void setItems(const QStringList &items);
    void setItems(const QVector<Item> &items);

    void setText(int row, const QString &text);
    void setStatus(int row, Item::Status status);

    void clearItems();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private slots:
    void frameChanged();

private:
    bool anyLoading() const;
    void updateMovieStatus();

    QVector<Item> m_items;

    const QPixmap m_pixmapSucceeded { ":/loganalyzer/icons/succeeded.png" };
    const QPixmap m_pixmapFailed { ":/loganalyzer/icons/failed.png" };
    const QPixmap m_pixmapEmpty { [this](){
        QPixmap pixmap(m_pixmapSucceeded.size());
        pixmap.fill();
        return pixmap;
    }() };
    QMovie m_movieLoading { ":/loganalyzer/icons/loading.gif" };
};
