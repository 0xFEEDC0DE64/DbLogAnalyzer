#pragma once

#include <QAbstractListModel>
#include <QList>

#include <utility>

class Q_CORE_EXPORT ChecklistModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ChecklistModel(QObject *parent = nullptr);
    explicit ChecklistModel(const QStringList &items, QObject *parent = nullptr);
    explicit ChecklistModel(const QList<std::pair<QString, bool> > &strings, QObject *parent = nullptr);

    QStringList items() const;
    void setItems(const QStringList &items);
    void setItems(const QList<std::pair<QString, bool> > &items);
    QStringList enabledItems() const;
    QStringList disabledItems() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;
    bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<std::pair<QString, bool> >  m_items;
};
