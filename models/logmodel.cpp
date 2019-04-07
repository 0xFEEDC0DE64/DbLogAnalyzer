#include "logmodel.h"

LogModel::LogModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return {};

    return QSqlQueryModel::headerData(section, orientation, role);
}
