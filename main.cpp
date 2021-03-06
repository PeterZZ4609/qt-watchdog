#include "db/clouddb.h"
#include "db/localdb.h"
#include "mainwindow.h"
#include "sensor/gzwssensor.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QQuickView>
#include <QQuickWidget>
#include <QSqlQuery>
#include <QUrl>

#include <unistd.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char* argv[])
{
    /* 加载qrc资源 */
    Q_INIT_RESOURCE(qss);
    /* 使用软键盘插件 */
    qputenv("QT_IM_MODULE", QByteArray("Qt5Input"));

    QApplication a(argc, argv);

    /* 加载QSS样式 */
    QFile f(":/qss/Aquas.qss");
    f.open(QFile::ReadOnly);
    a.setStyleSheet(f.readAll());
    f.close();

    /* 初始化传感器 */
    GzwsSensor* s = new GzwsSensor;
    //    s->startRefresh();
    s->startFakeRefresh(50);
    /* 初始化数据库 */
    LocalDb* localDb = new LocalDb("./test.db");
    /* 初始化主窗口 */
    MainWindow w;
    w.setLocalDb(localDb).setGzwsSensor(s).startRefreshPlot().startWriteIntoDatabase();

    w.show();

    // 也不知道有没有用，佛系防卡
    a.processEvents();

    return a.exec();
}
