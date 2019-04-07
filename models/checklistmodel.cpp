#include "checklistmodel.h"

ChecklistModel::ChecklistModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

ChecklistModel::ChecklistModel(const QStringList &items, QObject *parent) :
    QAbstractListModel(parent)
{
    for (const auto &item : items)
        m_items.append({ item, item, true });
}

ChecklistModel::ChecklistModel(const QList<ChecklistItem> &items, QObject *parent) :
    QAbstractListModel(parent),
    m_items(items)
{
}

const QList<ChecklistModel::ChecklistItem> &ChecklistModel::items() const
{
    return m_items;
}

void ChecklistModel::setItems(const QList<ChecklistItem> &items)
{
    emit beginResetModel();

    m_items = items;

    emit endResetModel();
}

void ChecklistModel::setItems(const QStringList &items)
{
    emit beginResetModel();

    m_items.clear();

    for (const auto &item : items)
        m_items.append({ item, item, true });

    emit endResetModel();
}

QStringList ChecklistModel::itemTexts() const
{
    QStringList items;

    for (const auto &item : m_items)
        items.append(item.displayText);

    return items;
}

QVariantList ChecklistModel::itemDatas() const
{
    QVariantList items;

    for (const auto &item : m_items)
        items.append(item.data);

    return items;
}

QList<ChecklistModel::ChecklistItem> ChecklistModel::enabledItems() const
{
    QList<ChecklistModel::ChecklistItem> items;

    for (const auto &item : m_items)
        if (item.checked)
            items.append(item);

    return items;
}

QList<ChecklistModel::ChecklistItem> ChecklistModel::disabledItems() const
{
    QList<ChecklistModel::ChecklistItem> items;

    for (const auto &item : m_items)
        if (!item.checked)
            items.append(item);

    return items;
}

QStringList ChecklistModel::enabledTexts() const
{
    QStringList items;

    for (const auto &item : m_items)
        if (item.checked)
            items.append(item.displayText);

    return items;
}

QStringList ChecklistModel::disabledTexts() const
{
    QStringList items;

    for (const auto &item : m_items)
        if (!item.checked)
            items.append(item.displayText);

    return items;
}

QVariantList ChecklistModel::enabledItemDatas() const
{
    QVariantList items;

    for (const auto &item : m_items)
        if (item.checked)
            items.append(item.data);

    return items;
}

QVariantList ChecklistModel::disabledItemDatas() const
{
    QVariantList items;

    for (const auto &item : m_items)
        if (!item.checked)
            items.append(item.data);

    return items;
}

int ChecklistModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_items.count();
}

QVariant ChecklistModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_items.size())
        return {};

    const auto &item = m_items.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole: return item.displayText;
    case Qt::EditRole: return item.data;
    case Qt::CheckStateRole: return item.checked ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

bool ChecklistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= m_items.size())
        return false;

    auto &item = m_items[index.row()];

    switch (role)
    {
    case Qt::CheckStateRole:
        item.checked = value.toBool();
        emit dataChanged(index, index, { Qt::CheckStateRole });
        return true;
    }

    return false;
}

Qt::ItemFlags ChecklistModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractListModel::flags(index);

    return QAbstractListModel::flags(index) | Qt::ItemIsUserCheckable;
}
