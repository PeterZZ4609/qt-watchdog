#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include "sensor/gzwssensor.h"
#include "window/util/axistag.h"
#include "window/util/customtabstyle.h"
#include <QWidget>

namespace Ui {
class LogWindow;
}

class LogWindow : public QWidget {
    Q_OBJECT

public:
    explicit LogWindow(GzwsSensor* sensor, QWidget* parent = nullptr);
    ~LogWindow();

private slots:
    void refreshTemp();

private:
    Ui::LogWindow* ui;
    QCustomPlot* mPlot = nullptr;
    QPointer<QCPGraph> mGraphTemperature;
    QPointer<QCPGraph> mGraphHumidity;
    AxisTag* mTagTemperature = nullptr;
    AxisTag* mTagHumidity;
    QTimer mDataTimer;
    QTimer mTimeDisplayTimer;
    // Sensor data handler
    GzwsSensor* gzwsSensor = nullptr;
};

#endif // LOGWINDOW_H
