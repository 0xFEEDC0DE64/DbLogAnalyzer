#include "tablecreatorthread.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

const QStringList TableCreatorThread::m_tables { "Hosts", "Processes", "Filenames", "Threads", "Types", "Logs" };

TableCreatorThread::TableCreatorThread(QSqlDatabase &database, QObject *parent) :
    QThread(parent), m_database(database)
{
}

const QStringList &TableCreatorThread::tables()
{
    return m_tables;
}

void TableCreatorThread::run()
{
    int index { 0 };

    for (const QString tableName : m_tables)
    {
        if (isInterruptionRequested())
            return;

        const auto sql = [&tableName,type=m_database.driverName()]() -> QString {
            if (tableName == "Logs")
            {
                if (type == "QSQLITE")
                {
                    return "CREATE TABLE IF NOT EXISTS `Logs` ("
                               "`ID` INTEGER PRIMARY KEY, "
                               "`Timestamp` TEXT NOT NULL, "
                               "`HostID` INTEGER NOT NULL, "
                               "`ProcessID` INTEGER NOT NULL, "
                               "`FilenameID` INTEGER NOT NULL, "
                               "`ThreadID` INTEGER NOT NULL, "
                               "`TypeID` INTEGER NOT NULL, "
                               "`Message` TEXT NOT NULL, "
                               "FOREIGN KEY (`HostID`) REFERENCES `Hosts`(`ID`), "
                               "FOREIGN KEY (`ProcessID`) REFERENCES `Processes`(`ID`), "
                               "FOREIGN KEY (`FilenameID`) REFERENCES `Filenames`(`ID`), "
                               "FOREIGN KEY (`ThreadID`) REFERENCES `Threads`(`ID`), "
                               "FOREIGN KEY (`TypeID`) REFERENCES `Types`(`ID`)"
                           ");";
                }
                else if (type == "QMYSQL")
                {
                    return "CREATE TABLE IF NOT EXISTS `Logs` ("
                               "`ID` INT UNSIGNED NOT NULL AUTO_INCREMENT, "
                               "`Timestamp` DATETIME NOT NULL, "
                               "`HostID` INT UNSIGNED NOT NULL, "
                               "`ProcessID` INT UNSIGNED NOT NULL, "
                               "`FilenameID` INT UNSIGNED NOT NULL, "
                               "`ThreadID` INT UNSIGNED NOT NULL, "
                               "`TypeID` INT UNSIGNED NOT NULL, "
                               "`Message` LONGTEXT NOT NULL, "
                               "PRIMARY KEY(`ID`), "
                               "INDEX(`Timestamp`), "
                               "FOREIGN KEY (`HostID`) REFERENCES `Hosts`(`ID`), "
                               "FOREIGN KEY (`ProcessID`) REFERENCES `Processes`(`ID`), "
                               "FOREIGN KEY (`FilenameID`) REFERENCES `Filenames`(`ID`), "
                               "FOREIGN KEY (`ThreadID`) REFERENCES `Threads`(`ID`), "
                               "FOREIGN KEY (`TypeID`) REFERENCES `Types`(`ID`)"
                           ");";
                }
            }
            else
            {
                if (type == "QSQLITE")
                {
                    return QString("CREATE TABLE IF NOT EXISTS `%0` ("
                                   "`ID` INTEGER PRIMARY KEY, "
                                   "`Name` TEXT NOT NULL UNIQUE"
                                   ");").arg(tableName);
                }
                else if (type == "QMYSQL")
                {
                    return QString("CREATE TABLE IF NOT EXISTS `%0` ("
                                   "`ID` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY, "
                                   "`Name` VARCHAR(255) NOT NULL UNIQUE"
                                   ");").arg(tableName);
                }
            }

            qFatal("unknown database type %s", qPrintable(type));
        }();

        QSqlQuery query(sql, m_database);
        if (query.lastError().isValid())
        {
            qCritical() << query.lastError().text();
            return;
        }

        sleep(1);
        emit someSignal(index++);
    }
}
