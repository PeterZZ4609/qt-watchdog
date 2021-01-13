#include "window/logwindow.h"
#include "ui_logwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QQuickWidget>
#include <QSharedPointer>

LogWindow::LogWindow(GzwsSensor* sensor, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LogWindow)
    , gzwsSensor(sensor)
{
    ui->setupUi(this);

    /* Use custom tab style */
    ui->tab->tabBar()->setStyle(new CustomTabStyle);

    class TimeRefreshWorker : public QThread {
    private:
        LogWindow* logWindow;

    public:
        TimeRefreshWorker(LogWindow* p)
            : logWindow(p)
        {
        }

    protected:
        void run()
        {
            logWindow->ui->timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
            msleep(200);
        }
    };

    /* time refresh */
    (new TimeRefreshWorker(this))->start();
    connect(&this->mTimeDisplayTimer, &QTimer::timeout, this, [=]() {
        this->ui->timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    });
    this->mTimeDisplayTimer.start(1000);

    mPlot = ui->mPlot;

    // dragable
    mPlot->setInteraction(QCP::iRangeDrag, true);

    // xAxis show datetime
    QTime time = QTime::currentTime();
    int nowMSec = time.msecsSinceStartOfDay();

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");

    mPlot->xAxis->setTicker(timeTicker);
    mPlot->xAxis->ticker()->setTickCount(10);
    mPlot->xAxis->setTickLabelRotation(30);

    // configure axes:
    mPlot->yAxis->setTickLabels(false);
    // left axis only mirrors inner right axis
    void (QCPAxis::*r)(const QCPRange&) = &QCPAxis::rangeChanged;
    void (QCPAxis::*r2)(const QCPRange&) = &QCPAxis::setRange;
    connect(mPlot->yAxis2, r, mPlot->yAxis, r2);

    // yAxis:
    mPlot->yAxis2->setVisible(true);
    mPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(50); // add some padding to have space for tags
    mPlot->axisRect()->addAxis(QCPAxis::atRight); // additional yAxis
    mPlot->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(50); // add some padding to have space for tags

    // create graphs:
    mGraphTemperature = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 0));
    mGraphHumidity = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 1));
    mGraphTemperature->setPen(QPen(QColor(250, 120, 0)));
    mGraphHumidity->setPen(QPen(QColor(0, 180, 60)));

    // create tags with newly introduced AxisTag class (see axistag.h/.cpp):
    mTagTemperature = new AxisTag(mGraphTemperature->valueAxis());
    mTagHumidity = new AxisTag(mGraphHumidity->valueAxis());
    mTagTemperature->setPen(mGraphTemperature->pen());
    mTagHumidity->setPen(mGraphHumidity->pen());

    // refreshTemp() per 50ms
    connect(&mDataTimer, &QTimer::timeout, this, &LogWindow::refreshTemp);
    mDataTimer.setTimerType(Qt::PreciseTimer);
    mDataTimer.start(200); // frequency
}

LogWindow::~LogWindow()
{
    delete ui;
}

void LogWindow::refreshTemp()
{
    qreal* data = gzwsSensor->getTempAndRhMoc();

    mGraphTemperature->addData(gzwsSensor->now.msecsSinceStartOfDay() * 0.001, data[0]);
    mGraphHumidity->addData(gzwsSensor->now.msecsSinceStartOfDay() * 0.001, data[1]);
    // scroll xAxis
    mPlot->xAxis->rescale();
    mGraphTemperature->rescaleValueAxis(false, true);
    mGraphHumidity->rescaleValueAxis(false, true);
    mPlot->xAxis->setRange(mPlot->xAxis->range().upper, 50, Qt::AlignRight);

    // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
    double yValue = mGraphTemperature->dataMainValue(mGraphTemperature->dataCount() - 1);
    mTagTemperature->updatePosition(yValue);
    mTagTemperature->setText(QString::number(yValue, 'f', 1).append("℃"));
    double yValue2 = mGraphHumidity->dataMainValue(mGraphHumidity->dataCount() - 1);
    mTagHumidity->updatePosition(yValue2);
    mTagHumidity->setText(QString::number(yValue2, 'f', 1).append("%"));

    delete[] data;

    mPlot->replot();
}
