#include "mainwindow.h"
#include "ui_logwindow.h"
#include "window/gzwshistorywindow.h"

#include <QDateTime>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QQuickWidget>
#include <QSharedPointer>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LogWindow)
{
    ui->setupUi(this);

    /* Use custom tab style */
    ui->tab->tabBar()->setStyle(new CustomTabStyle);

    mPlot = ui->mPlot;
    // 可拖动
    mPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    // 设置横轴刻度标签，这时候x轴给的值应该是时间，单位是秒，类型是double
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    mPlot->xAxis->setTicker(timeTicker);
    mPlot->xAxis->ticker()->setTickCount(10); // 大刻度的数量
    mPlot->xAxis->setTickLabelRotation(30);
    // 左y轴镜像右y轴
    mPlot->yAxis->setTickLabels(false); // 左y轴不显示标签
    void (QCPAxis::*r)(const QCPRange&) = &QCPAxis::rangeChanged;
    void (QCPAxis::*r2)(const QCPRange&) = &QCPAxis::setRange;
    connect(mPlot->yAxis2, r, mPlot->yAxis, r2);
    // y轴
    mPlot->yAxis2->setVisible(true);
    mPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(50);
    mPlot->axisRect()->addAxis(QCPAxis::atRight); // 额外的y轴
    mPlot->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(50);
    // 加折线，设定折线依附的x和y轴，设定折线颜色
    mGraphTemperature = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 0));
    mGraphHumidity = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 1));
    mGraphTemperature->setPen(QPen(QColor(250, 120, 0)));
    mGraphHumidity->setPen(QPen(QColor(0, 180, 60)));
    // 官方提供的漂亮的AxisTag，设置tag关联的axis，设置tag外观颜色
    mTagTemperature = new AxisTag(mGraphTemperature->valueAxis());
    mTagHumidity = new AxisTag(mGraphHumidity->valueAxis());
    mTagTemperature->setPen(mGraphTemperature->pen());
    mTagHumidity->setPen(mGraphHumidity->pen());

    //    QCPItemTracer t = mPlot->addGraph()

    /* 刷新时间 */
    connect(&this->mTimeDisplayTimer, &QTimer::timeout, this, [=]() {
        ui->timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
    });
    this->mTimeDisplayTimer.start(0);

    /* tab-系统信息 */
    connect(&mSystemEnvironmentRefreshTimer, &QTimer::timeout, this, [=]() {
        /* 本地数据库 */
        // 数据库驱动
        static bool dbInfoSetted = false;
        if (!dbInfoSetted && this->localDb != nullptr) {
            this->ui->label_localdb_driver->setText(this->localDb->driverName());
            this->ui->label_localdb_path->setText(QFileInfo(this->localDb->dbName()).absoluteFilePath());
            // 可能是不同线程的原因
            for (QString s : LocalDb(this->localDb->dbName()).tables()) {
                this->ui->listWidget->addItem(s);
            }
            dbInfoSetted = true;
        }
        this->ui->label_localdb_size->setText(
            QString::number((this->localDb->dbSize() * 1.0) / 1024).append("KB"));
    });
    mSystemEnvironmentRefreshTimer.start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshPlot()
{
    // 加值
    mGraphTemperature->addData(
        gzwsSensor->now().time().msecsSinceStartOfDay() * 0.001, gzwsSensor->temperature());
    mGraphHumidity->addData(
        gzwsSensor->now().time().msecsSinceStartOfDay() * 0.001, gzwsSensor->humidity());
    // 滚动x轴
    mPlot->xAxis->rescale();
    mGraphTemperature->rescaleValueAxis(false, false);
    mGraphHumidity->rescaleValueAxis(false, false);
    mPlot->xAxis->setRange(mPlot->xAxis->range().upper, 50 /* 50s */, Qt::AlignRight);

    // update the vertical axis tag positions and texts to match the rightmost data point of the graphs:
    double yValue = mGraphTemperature->dataMainValue(mGraphTemperature->dataCount() - 1);
    mTagTemperature->updatePosition(yValue);
    mTagTemperature->setText(QString::number(yValue, 'f', 1).append("℃"));
    double yValue2 = mGraphHumidity->dataMainValue(mGraphHumidity->dataCount() - 1);
    mTagHumidity->updatePosition(yValue2);
    mTagHumidity->setText(QString::number(yValue2, 'f', 1).append("%"));

    mPlot->replot();

    quint32 lux = gzwsSensor->lux();
    ui->label_lux->setText(QString("%1 lux").arg(lux));
    ui->luxBar->setValue(lux);
}

void MainWindow::writeDb()
{
    this->localDb->putGzws(
        gzwsSensor->lux(),
        gzwsSensor->temperature(),
        gzwsSensor->humidity(),
        gzwsSensor->now().toMSecsSinceEpoch());
}

void MainWindow::on_historyButton_clicked()
{
    int left = (this->geometry().width() - s.geometry().width()) / 2;
    int top = (this->geometry().height() - s.geometry().height()) / 2;
    s.move(left, top);
    s.exec();
}

MainWindow& MainWindow::startRefreshPlot()
{
    connect(&mDataTimer, &QTimer::timeout, this, &MainWindow::refreshPlot);
    mDataTimer.setTimerType(Qt::PreciseTimer);
    mDataTimer.start(200); // frequency
    return *this;
}

MainWindow& MainWindow::startWriteIntoDatabase()
{
    connect(&mDbWritingTimer, &QTimer::timeout, this, &MainWindow::writeDb);
    mDbWritingTimer.setTimerType(Qt::PreciseTimer);
    mDbWritingTimer.start(1000); // frequency
    return *this;
}
