#pragma once

#include <QSqlRelationalTableModel>

class SqlRelationalTableModel : public QSqlRelationalTableModel
{
    Q_OBJECT

public:
    explicit SqlRelationalTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};
