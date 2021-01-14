#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include "sensor/gzwssensor.h"
#include "window/gzwshistorywindow.h"
#include "window/util/axistag.h"
#include "window/util/customtabstyle.h"
#include <QWidget>

namespace Ui {
class LogWindow;
}

class MainWindow : public QWidget {
    Q_OBJECT

private:
    /* UI */
    Ui::LogWindow* ui;
    QCustomPlot* mPlot = nullptr;
    QPointer<QCPGraph> mGraphTemperature;
    QPointer<QCPGraph> mGraphHumidity;
    AxisTag* mTagTemperature = nullptr;
    AxisTag* mTagHumidity;
    /* dragging flag */
    bool dragging = false;

    /* timer */
    QTimer mDataTimer; // 刷新图表的timer
    QTimer mTimeDisplayTimer; // 刷新时间的timer
    QTimer mDbWritingTimer; // 写数据库的timer
    QTimer mSystemEnvironmentRefreshTimer; // 刷新系统信息
    QTimer mVideoCaptureTimer; // 监控

    // data handler
    GzwsSensor* gzwsSensor = nullptr;
    LocalDb* localDb = nullptr;

    // sub windows
    GzwsHistoryWindow s;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    /* setters */
    MainWindow& setGzwsSensor(GzwsSensor* sensor) { return this->gzwsSensor = sensor, *this; }
    MainWindow& setLocalDb(LocalDb* localDb) { return this->localDb = localDb, this->s.setLocalDb(localDb), *this; }
    /* timer functions */
    MainWindow& startRefreshPlot(); // 刷新图表
    MainWindow& startWriteIntoDatabase(); // 记录到数据库

private slots:
    /* refresh the plot */
    void refreshPlot();
    void writeDb();
    /* open gzws history dialog */
    void on_historyButton_clicked();
    void on_pushButton_clicked();
};

#endif // LOGWINDOW_H
