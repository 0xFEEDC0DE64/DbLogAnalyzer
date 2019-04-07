#pragma once

#include <QSqlQueryModel>

class LogModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit LogModel(QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};
