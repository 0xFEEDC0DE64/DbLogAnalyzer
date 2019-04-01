#include "checklistmodel.h"

ChecklistModel::ChecklistModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

ChecklistModel::ChecklistModel(const QStringList &items, QObject *parent) :
    QAbstractListModel(parent)
{
    for (const auto &item : items)
        m_items.append(std::make_pair(item, true));
}

ChecklistModel::ChecklistModel(const QList<std::pair<QString, bool> > &items, QObject *parent) :
    QAbstractListModel(parent),
    m_items(items)
{
}

QStringList ChecklistModel::items() const
{
    QStringList items;

    for (const auto &pair : m_items)
        items.append(std::get<0>(pair));

    return items;
}

void ChecklistModel::setItems(const QStringList &items)
{
    emit beginResetModel();

    m_items.clear();

    for (const auto &item : items)
        m_items.append(std::make_pair(item, true));

    emit endResetModel();
}

void ChecklistModel::setItems(const QList<std::pair<QString, bool> > &items)
{
    emit beginResetModel();

    m_items = items;

    emit endResetModel();
}

QStringList ChecklistModel::enabledItems() const
{
    QStringList items;

    for (const auto &pair : m_items)
        if (std::get<1>(pair))
            items.append(std::get<0>(pair));

    return items;
}

QStringList ChecklistModel::disabledItems() const
{
    QStringList items;

    for (const auto &pair : m_items)
        if (!std::get<1>(pair))
            items.append(std::get<0>(pair));

    return items;
}

int ChecklistModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_items.count();
}

QModelIndex ChecklistModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (!idx.isValid() || column != 0 || row >= m_items.count() || row < 0)
        return QModelIndex();
    return createIndex(row, 0);
}

QMap<int, QVariant> ChecklistModel::itemData(const QModelIndex &index) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid))
        return QMap<int, QVariant>{};
    const auto &item = m_items.at(index.row());
    return QMap<int, QVariant>{{
        std::make_pair<int>(Qt::DisplayRole, std::get<0>(item)),
        std::make_pair<int>(Qt::EditRole, std::get<0>(item)),
        std::make_pair<int>(Qt::CheckStateRole, std::get<1>(item) ? Qt::Checked : Qt::Unchecked)
        }};
}

bool ChecklistModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    if (roles.isEmpty())
        return false;
    if (std::any_of(roles.keyBegin(), roles.keyEnd(), [](int role) { return role != Qt::CheckStateRole; }))
        return false;
    auto roleIter = roles.constFind(Qt::CheckStateRole);
    Q_ASSERT(roleIter != roles.constEnd());
    return setData(index, roleIter.value(), roleIter.key());
}

QVariant ChecklistModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_items.size())
        return QVariant();
    const auto &item = m_items.at(index.row());
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return std::get<0>(item);
    if (role == Qt::CheckStateRole)
        return std::get<1>(item) ? Qt::Checked : Qt::Unchecked;
    return QVariant();
}

bool ChecklistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 && index.row() < m_items.size() && role == Qt::CheckStateRole)
    {
        auto &item = m_items[index.row()];
        std::get<1>(item) = value.toBool();
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
