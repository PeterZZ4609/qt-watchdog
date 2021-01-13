
#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include <sys/termios.h>

#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>
#include <QString>
#include <QThread>
#include <QTime>
#include <QTimer>

class GzwsSensor : public QObject {
public:
    GzwsSensor();

    /* initialize the file descriptor fot r/w rs485 */
    bool init();
    /* starting refresh() timer */
    void startRefresh();
    /* for test */
    void test();
    /* temperature & relative humidity */
    qreal* getTempAndRh();
    qreal* getTempAndRhMoc();
    /* lux */
    quint32 getLux();

public slots:
    void refresh();

public:
    QTime now;

private:
    const QString dev = "/dev/ttyO1";
    const quint8 code_humi_temp[8] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b };
    const quint8 code_lux[8] = { 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xcb };
    const quint8 code_humi_temp_lux[8] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08 };

    qint32 fd = -1;
    bool inited = false;

    qreal tempAndRh[2] = { 20.0, 16.0 };
    quint32 lux = 0;
    QReadWriteLock locker;
    QTimer timer;
};

#endif // SENSORCONTROLLER_H
