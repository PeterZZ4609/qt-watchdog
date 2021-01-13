#include "db/clouddb.h"
#include "db/localdb.h"
#include "mainwindow.h"
#include "sensor/gzwssensor.h"
#include "window/logwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QQuickView>
#include <QQuickWidget>
#include <QSqlQuery>

#include <unistd.h>

// extern char** environ;

//void DesktopInputPanel::show()
//{
//    AppInputPanel::show();
//    Q_D(DesktopInputPanel);
//    if (d->view) {
//        QRect rc=QGuiApplication::primaryScreen()->geometry();

//        rc.moveTo(rc.width()*1/6,rc.height()*1/3);
//        rc.setWidth(rc.width()*2/3);
//        rc.setHeight(rc.height()*2/3);
//        repositionView(rc);
//        d->view->show();
//    }
//}

int main(int argc, char* argv[])
{
    //    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_IM_MODULE", QByteArray("Qt5Input"));

    //    char** var;
    //    for (var = environ; *var != NULL; var++)
    //        qDebug() << *var;

    /* Virtual Keyboard */
    QApplication a(argc, argv);

    /* load qss */
    //    QFile f("/root/sdcard/QSS/test.qss");
    //    f.open(QFile::ReadOnly);
    //    a.setStyleSheet(f.readAll());
    //    f.close();

    a.processEvents();

    GzwsSensor* s = new GzwsSensor;
    //    s->startRefresh();
    LogWindow w(s);

    w.show();

    return a.exec();
}
