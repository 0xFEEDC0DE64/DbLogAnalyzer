#include "graphdialog.h"
#include "ui_graphdialog.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>

GraphDialog::GraphDialog(QSqlDatabase &database, QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_unique<Ui::GraphDialog>()),
    m_database(database)
{
    m_ui->setupUi(this);

    QString sql;

    if (m_database.driverName() == "QSQLITE")
    {
        sql = "SELECT "
                  "`Timestamp`, "
                  "COUNT(`Timestamp`) "
              "FROM "
                  "`Logs` "
              "GROUP BY "
                  "STRFTIME('%Y-%m-%d %H:00:00.000', `Timestamp`)";
    }
    else if (m_database.driverName() == "QMYSQL")
    {
        sql = "SELECT "
                  "`Timestamp`, "
                  "COUNT(`Timestamp`) "
              "FROM "
                  "`Logs` "
              "GROUP BY "
                  "DATE_FORMAT(`Timestamp`, '%Y-%m-%d %H:%i:00.000')";
    }
    else
        qFatal("unknown sql driver");

    QSqlQuery query(sql, m_database);
    if (query.lastError().isValid())
        qCritical() << query.lastError().text();

    auto chart = new QtCharts::QChart();
    chart->legend()->hide();
    chart->setTitle(tr("Charts-Test"));

    auto series = new QtCharts::QLineSeries();
    chart->addSeries(series);

    QDateTime minDt, maxDt;
    int maxCount{};

    while (query.next())
    {
        const auto timestampStr = query.value(0).toString();
        qDebug() << timestampStr;

        const auto timestamp = QDateTime::fromString(timestampStr, QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz"));
        Q_ASSERT(timestamp.isValid());

        if (minDt.isNull() || timestamp < minDt)
            minDt = timestamp;

        if (maxDt.isNull() || timestamp > maxDt)
            maxDt = timestamp;

        const auto count = query.value(1).toInt();

        if (count > maxCount)
            maxCount = count;

        qDebug() << timestamp << count;

        series->append(timestamp.toMSecsSinceEpoch(), count);
    }

    qDebug() << minDt << maxDt;

    auto axisX = new QtCharts::QDateTimeAxis;
    axisX->setRange(minDt, maxDt);
    axisX->setTickCount(20);
    axisX->setFormat("HH:mm:ss");
    axisX->setTitleText("Timestamp");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto axisY = new QtCharts::QValueAxis;
    axisY->setMax(maxCount);
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Logs count");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    m_ui->chartView->setRenderHint(QPainter::Antialiasing);
    m_ui->chartView->setChart(chart);
}

GraphDialog::~GraphDialog() = default;
