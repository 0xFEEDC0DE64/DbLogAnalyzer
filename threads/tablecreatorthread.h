#pragma once

#include <QThread>

class QSqlDatabase;

class TableCreatorThread : public QThread
{
    Q_OBJECT

    static const QStringList m_tables;

public:
    explicit TableCreatorThread(QSqlDatabase &database, QObject *parent = nullptr);

    static const QStringList &tables();

signals:
    void someSignal(int index);

protected:
    void run() override;

private:
    QSqlDatabase &m_database;
};
