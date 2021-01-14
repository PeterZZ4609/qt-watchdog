
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
#include <QtSerialPort/QSerialPort>

class GzwsSensor : public QObject {
public:
    GzwsSensor() = default;

    /* initialize the file descriptor fot r/w rs485 */
    bool init();
    /* starting refresh() timer */
    void startRefresh();
    void startRefresh(int msec);
    void startFakeRefresh(int msec);
    /* data getters */
    qreal temperature();
    qreal humidity();
    quint32 lux();
    QDateTime now();

public slots:
    void refresh();

private:
    const QString dev = "/dev/ttyO1";
    // 应该怎么解决 unsigned char 到 char 的转换呢？
    //    const char _code_humi_temp[8] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xc4, 0x0b };
    //    const char _code_lux[8] = { 0x01, 0x03, 0x00, 0x02, 0x00, 0x02, 0x65, 0xcb };
    const char _code_humi_temp_lux[8] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08 };
    //    QByteArray* code_humi_temp = new QByteArray(_code_humi_temp, 8);
    //    QByteArray* code_lux = new QByteArray(_code_lux, 8);
    QByteArray* code_humi_temp_lux = new QByteArray(_code_humi_temp_lux, 8);

    QSerialPort* qsp;
    bool inited = false;

    /* ====== safe area ====== */
    QDateTime _now = QDateTime::currentDateTime();
    qreal _temperature = 20.0;
    qreal _humidity = 20.0;
    quint32 _lux = 500;
    QReadWriteLock locker;
    /* ======================= */
    QTimer timer;
};

#endif // SENSORCONTROLLER_H
