#include "progressmodel.h"

#include <QImage>

#include <algorithm>

ProgressModel::ProgressModel(QObject *parent) :
    QAbstractListModel(parent)
{
    connect(&m_movieLoading, &QMovie::frameChanged, this, &ProgressModel::frameChanged);
}

ProgressModel::ProgressModel(const QStringList &items, QObject *parent) :
    QAbstractListModel(parent)
{
    connect(&m_movieLoading, &QMovie::frameChanged, this, &ProgressModel::frameChanged);

    m_items.reserve(items.count());
    for (const auto &item : items)
        m_items.append({ item, Item::Status::None });
}

ProgressModel::ProgressModel(const QVector<ProgressModel::Item> &items, QObject *parent) :
    QAbstractListModel(parent),
    m_items(items)
{
    connect(&m_movieLoading, &QMovie::frameChanged, this, &ProgressModel::frameChanged);

    if (anyLoading())
        m_movieLoading.start();
}

const QVector<ProgressModel::Item> &ProgressModel::items() const
{
    return m_items;
}

void ProgressModel::setItems(const QStringList &items)
{
    emit beginResetModel();
    m_items.clear();
    m_items.reserve(items.count());
    for (const auto &item : items)
        m_items.append({ item, Item::Status::None });
    emit endResetModel();

    m_movieLoading.stop();
}

void ProgressModel::setItems(const QVector<ProgressModel::Item> &items)
{
    emit beginResetModel();
    m_items = items;
    emit endResetModel();

    updateMovieStatus();
}

void ProgressModel::setText(int row, const QString &text)
{
    m_items[row].text = text;

    const auto index = createIndex(row, 0);
    emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });
}

void ProgressModel::setStatus(int row, ProgressModel::Item::Status status)
{
    m_items[row].status = status;

    const auto index = createIndex(row, 0);
    emit dataChanged(index, index, { Qt::DecorationRole });

    if (status == Item::Status::Loading)
        m_movieLoading.start();
    else
        updateMovieStatus();
}

void ProgressModel::clearItems()
{
    emit beginResetModel();
    m_items.clear();
    emit endResetModel();

    m_movieLoading.stop();
}

int ProgressModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_items.count();
}

QVariant ProgressModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_items.size())
        return {};

    const auto &item = m_items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return item.text;
    case Qt::DecorationRole:
        switch (item.status)
        {
        case Item::Status::None:
            return m_pixmapEmpty;
        case Item::Status::Loading:
            return m_movieLoading.currentPixmap();
        case Item::Status::Succeeded:
            return m_pixmapSucceeded;
        case Item::Status::Failed:
            return m_pixmapFailed;
        }
    }

    return {};
}

void ProgressModel::frameChanged()
{
    for (auto iter = m_items.constBegin(); iter != m_items.constEnd(); iter++)
    {
        if (iter->status == Item::Status::Loading)
        {
            const auto index = createIndex(std::distance(m_items.constBegin(), iter), 0);
            emit dataChanged(index, index, { Qt::DecorationRole });
        }
    }
}

bool ProgressModel::anyLoading() const
{
    return std::any_of(m_items.constBegin(), m_items.constEnd(), [](const auto &item){ return item.status == Item::Status::Loading; });
}

void ProgressModel::updateMovieStatus()
{
    if (anyLoading())
        m_movieLoading.start();
    else
        m_movieLoading.stop();
}
