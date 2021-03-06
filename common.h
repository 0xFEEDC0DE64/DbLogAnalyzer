#pragma once

#include <QString>
#include <QHash>
#include <QDate>
#include <QtGlobal>
#include <QMetaType>
#include <QMap>
#include <QSqlDatabase>

#include <algorithm>

using Host = QString;
using Process = QString;
using Filename = QString;
struct Logfile {
    QString filename;
    QString filepath;
    qint64 filesize;
    bool gzipCompressed;
};
using ScanResult = QHash<Host, QHash<Process, QHash<QDate, Logfile> > >;
Q_DECLARE_METATYPE(ScanResult)

inline bool scanResultEmpty(const ScanResult &result)
{
    return result.isEmpty() || std::all_of(result.constBegin(), result.constEnd(), [](const auto &host){
        return host.isEmpty() || std::all_of(host.constBegin(), host.constEnd(), [](const auto &process){
            return process.isEmpty();
        });
    });
}

struct Project {
    QMap<int, QString> hosts;
    QMap<int, QString> processes;
    QMap<int, QString> filenames;
    QMap<int, QString> threads;
    QMap<int, QString> types;
    QSqlDatabase database;
};
