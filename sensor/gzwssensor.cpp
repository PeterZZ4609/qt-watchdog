#include "gzwssensor.h"
#include <QDebug>

qreal GzwsSensor::temperature()
{
    this->locker.lockForRead();
    qreal ret = this->_temperature;
    this->locker.unlock();
    return ret;
}

qreal GzwsSensor::humidity()
{
    this->locker.lockForRead();
    qreal ret = this->_humidity;
    this->locker.unlock();
    return ret;
}

quint32 GzwsSensor::lux()
{
    this->locker.lockForRead();
    quint32 ret = this->_lux;
    this->locker.unlock();
    return ret;
}

QDateTime GzwsSensor::now()
{
    this->locker.lockForRead();
    QDateTime ret = this->_now;
    this->locker.unlock();
    return ret;
}

bool GzwsSensor::init()
{

    /* Qt Serial Port Initialization */
    qsp = new QSerialPort(dev);
    qsp->open(QIODevice::ReadWrite);
    qsp->setBaudRate(QSerialPort::Baud9600);
    qsp->setDataBits(QSerialPort::Data8);
    qsp->setParity(QSerialPort::NoParity);
    qsp->setStopBits(QSerialPort::OneStop);
    qsp->setFlowControl(QSerialPort::NoFlowControl);

    inited = true;

    return true;
}

void GzwsSensor::startRefresh()
{
    startRefresh(200);
}

void GzwsSensor::startRefresh(int msec)
{
    if (!inited)
        init();
    connect(&this->timer, &QTimer::timeout, this, &GzwsSensor::refresh);
    this->timer.start(msec);
}

void GzwsSensor::startFakeRefresh(int msec)
{
    connect(&this->timer, &QTimer::timeout, this, [=]() {
        this->locker.lockForWrite();
        this->_now = QDateTime::currentDateTime();
        this->_temperature = (qrand() % 100 + 200) * 0.1;
        this->_humidity = (qrand() % 100 + 300) * 0.1;
        this->_lux = qrand() % 200 + 400;
        this->locker.unlock();
    });
    this->timer.start(msec);
}

void GzwsSensor::refresh()
{
    static quint32 failed = 0;
    //    qDebug() << "GzwsSensor is refresing ...";
    /* send query code */
    qsp->write(*code_humi_temp_lux);
    //    qDebug().noquote() << QString("write %1").arg(ret);
    QByteArray buf = qsp->readAll();
    if (buf.size() != 19) {
        ++failed;
        qDebug().noquote() << QString("GzwsSensor refresh failed %1").arg(failed);
        return;
    }
    // debug接收到的数据
    //    QString log;
    //    for (int i = 0; i < buf.size(); ++i) {
    //        log.append(QString().sprintf("%02x ", buf.at(i)));
    //    }
    //    log.append(QString().sprintf("( %d bytes )", buf.size()));
    //    qDebug().noquote() << log;

    /* parse temperature */
    quint16 temp = buf[3];
    temp <<= 8;
    temp += buf[4];
    qreal rh = static_cast<qint16>(temp) * 1.0 / 10;
    /* parse humidity */
    temp = buf[5];
    temp <<= 8;
    temp += buf[6];
    qreal t = static_cast<qint16>(temp) * 1.0 / 10;
    // parse lux
    quint32 lux = buf[7];
    lux <<= 8;
    lux += buf[8];
    lux <<= 8;
    lux += buf[9];
    lux <<= 8;
    lux += buf[10];

    //    qDebug().noquote() << QString("%1 %2lux %3C %4%").arg(_now.toMSecsSinceEpoch()).arg(lux).arg(t).arg(rh);

    QDateTime now = QDateTime::currentDateTime();
    // update data三
    this->locker.lockForWrite();
    this->_now = now;
    this->_temperature = t;
    this->_humidity = rh;
    this->_lux = lux;
    this->locker.unlock();
}
