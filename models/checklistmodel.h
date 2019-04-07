#pragma once

#include <QAbstractListModel>
#include <QList>

class Q_CORE_EXPORT ChecklistModel : public QAbstractListModel
{
    Q_OBJECT

public:
    struct ChecklistItem
    {
        QString displayText;
        QVariant data;
        bool checked;
    };

    explicit ChecklistModel(QObject *parent = nullptr);
    explicit ChecklistModel(const QStringList &items, QObject *parent = nullptr);
    explicit ChecklistModel(const QList<ChecklistItem> &strings, QObject *parent = nullptr);

    const QList<ChecklistItem> &items() const;
    void setItems(const QList<ChecklistItem> &items);
    void setItems(const QStringList &items);

    QStringList itemTexts() const;
    QVariantList itemDatas() const;
    QList<ChecklistItem> enabledItems() const;
    QList<ChecklistItem> disabledItems() const;
    QStringList enabledTexts() const;
    QStringList disabledTexts() const;
    QVariantList enabledItemDatas() const;
    QVariantList disabledItemDatas() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QList<ChecklistItem>  m_items;
};
