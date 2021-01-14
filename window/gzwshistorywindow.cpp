#include "gzwshistorywindow.h"
#include "ui_gzwshistorywindow.h"

GzwsHistoryWindow::GzwsHistoryWindow(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::GzwsHistoryWindow)
{
    ui->setupUi(this);
    connect(ui->dateTimeFrom, &QDateTimeEdit::dateTimeChanged, ui->dateTimeTo, [=]() {
        if (ui->dateTimeFrom->dateTime() > ui->dateTimeTo->dateTime()) {
            ui->dateTimeTo->setDateTime(ui->dateTimeFrom->dateTime());
        }
    });
    connect(ui->dateTimeTo, &QDateTimeEdit::dateTimeChanged, ui->dateTimeFrom, [=]() {
        if (ui->dateTimeFrom->dateTime() > ui->dateTimeTo->dateTime()) {
            ui->dateTimeFrom->setDateTime(ui->dateTimeTo->dateTime());
        }
    });
    ui->dateTimeFrom->setDateTime(QDateTime::currentDateTime());

    mPlot = ui->mPlot;

    mPlot->setInteraction(QCP::iRangeDrag, true);

    timeTicker = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    timeTicker->setDateTimeFormat("yyyy-MM-dd HH:mm:ss");
    mPlot->xAxis->setTicker(timeTicker);
    mPlot->xAxis->ticker()->setTickCount(10);
    mPlot->xAxis->setTickLabelRotation(30);

    mPlot->yAxis2->setVisible(true); // 温度y轴
    mPlot->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(30);
    mPlot->axisRect()->addAxis(QCPAxis::atRight); // 湿度y轴
    mPlot->axisRect()->axis(QCPAxis::atRight, 1)->setPadding(30);
    mPlot->axisRect()->addAxis(QCPAxis::atRight); // 光照度y轴
    mPlot->axisRect()->axis(QCPAxis::atRight, 2)->setPadding(30);

    mGraphTemperature = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 0));
    mGraphHumidity = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 1));
    mGraphLux = mPlot->addGraph(mPlot->xAxis, mPlot->axisRect()->axis(QCPAxis::atRight, 2));
    mGraphTemperature->setPen(QPen(QColor(250, 120, 0)));
    mGraphHumidity->setPen(QPen(QColor(0, 180, 60)));
    mGraphLux->setPen(QPen(QColor(0, 0, 250)));

    connect(ui->checkBox_temp, &QCheckBox::stateChanged, this, &GzwsHistoryWindow::checkBoxClicked);
    connect(ui->checkBox_lux, &QCheckBox::stateChanged, this, &GzwsHistoryWindow::checkBoxClicked);
    connect(ui->checkBox_rh, &QCheckBox::stateChanged, this, &GzwsHistoryWindow::checkBoxClicked);
}

GzwsHistoryWindow::~GzwsHistoryWindow()
{
    delete ui;
}

void GzwsHistoryWindow::refreshHistoryData()
{
    if (db == nullptr) {
        qDebug() << __FUNCTION__ << "No db handler!";
        return;
    }
    qint64 from = ui->dateTimeFrom->dateTime().toMSecsSinceEpoch();
    qint64 to = ui->dateTimeTo->dateTime().toMSecsSinceEpoch();
    QVector<gzws_log>* data = db->getGzws(from, to);
    if (data == nullptr || data->size() < 1) {
        return;
    }
    // 数据丢进去
    for (gzws_log log : *data) {
        mGraphTemperature->addData(log.ctime * 0.001, log.temperature);
        mGraphHumidity->addData(log.ctime * 0.001, log.humidity);
        mGraphLux->addData(log.ctime * 0.001, log.lux);
    }
    // 滚动x轴
    mPlot->xAxis->rescale();
    mGraphTemperature->rescaleValueAxis(false, true);
    mGraphHumidity->rescaleValueAxis(false, true);
    mGraphLux->rescaleValueAxis(false, true);
    double xMin = qMin(mGraphTemperature->dataMainKey(0), qMin(mGraphHumidity->dataMainKey(0), mGraphLux->dataMainKey(0)));
    double xMax = qMax(mGraphTemperature->dataMainKey(mGraphTemperature->dataCount() - 1),
        qMax(mGraphHumidity->dataMainKey(mGraphHumidity->dataCount() - 1),
            mGraphLux->dataMainKey(mGraphLux->dataCount() - 1)));
    mPlot->xAxis->setRange(xMin, xMax);
    // 标签不一定非要带年份
    if (QDateTime::fromSecsSinceEpoch(xMin).date().year() == QDateTime::fromSecsSinceEpoch(xMax).date().year()) {
        timeTicker->setDateTimeFormat("HH:mm:ss");
    }
    mGraphTemperature->valueAxis()->setAxisFormat("℃");
    mGraphHumidity->valueAxis()->setAxisFormat("%");
    mPlot->replot();

    // 滑块居中
    ui->slider->setValue(50);
    xMid = mPlot->xAxis->range().center();
    xMaxSize = mPlot->xAxis->range().size();

    // 检查下checkbox
    checkBoxClicked(0);
}

void GzwsHistoryWindow::on_refreshButton_clicked()
{
    refreshHistoryData();
}

void GzwsHistoryWindow::on_slider_valueChanged(int value)
{
    //    mPlot->xAxis->rescale();
    //    mGraphTemperature->rescaleValueAxis(false, true);
    //    mGraphHumidity->rescaleValueAxis(false, true);
    double mid = mPlot->xAxis->range().center();
    double t = qPow(1.2, value - 50); // 默认1

    double lower = mid - (xMaxSize * 0.5 / t);
    double upper = mid + (xMaxSize * 0.5 / t);

    mPlot->xAxis->setRange(lower, upper);
    mPlot->replot();
}

void GzwsHistoryWindow::checkBoxClicked(int value)
{
    bool temp = ui->checkBox_temp->isChecked();
    bool rh = ui->checkBox_rh->isChecked();
    bool lux = ui->checkBox_lux->isChecked();
    mGraphTemperature->setVisible(temp);
    mGraphHumidity->setVisible(rh);
    mGraphLux->setVisible(lux);
    mPlot->axisRect()->axis(QCPAxis::atRight, 0)->setVisible(temp);
    mPlot->axisRect()->axis(QCPAxis::atRight, 1)->setVisible(rh);
    mPlot->axisRect()->axis(QCPAxis::atRight, 2)->setVisible(lux);
    mPlot->replot();
}
