#include "localdb.h"

#include <QDebug>
#include <QFile>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVector>

LocalDb::LocalDb(QString dbName)
    : LocalDb()
{
    this->connect(dbName);
}

bool LocalDb::connect(QString dbName)
{
    if (connected) {
        qDebug() << "Already connected !";
        return false;
    }
    // QSqlDatabase open
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    this->_dbName = dbName;
    this->db.setDatabaseName(dbName);
    bool ret = db.open();
    if (ret) {
        qDebug().noquote() << QString("Database [%1] created.").arg(dbName);
    } else {
        qDebug().noquote() << QString("Database [%1] not created !").arg(dbName);
    }
    // create tables
    if (ret) {
        QSqlQuery q;
        q.exec(sqlCreateTbGzws);
        q.exec(sqlCreateTbSettings);
    }
    /* show tables */
    QStringList tables = db.tables();
    qDebug() << tables;
    if (ret)
        connected = true;
    return connected;
}

bool LocalDb::putGzws(quint32 lux, qreal temperature, qreal humidity, qint64 ctime)
{
    if (!connected) {
        qDebug() << "Need to connect a sqlite database file !";
        return false;
    }
    QSqlQuery q;
    QString sql = sqlInsertTbGzws.arg(lux).arg(temperature).arg(humidity).arg(ctime);
    bool ret = q.exec(sql);
    //    qDebug().noquote() << QString("sql(local): [%1] %2").arg(ret ? "Success" : "Failed").arg(sql);
    return ret;
}

QVector<gzws_log>* LocalDb::getGzws(qint64 from, qint64 to)
{
    QVector<gzws_log>* v = new QVector<gzws_log>;
    QSqlQuery q;
    QString sql = QString("select ctime,lux,tmp,rh from %1 where ctime>=%2 and ctime<=%3 order by ctime asc").arg(tbNameGzws).arg(from).arg(to);
    qDebug().noquote() << sql;
    bool ret = q.exec(sql);
    while (q.next()) {
        gzws_log* log = new gzws_log;
        log->ctime = q.value(0).toULongLong();
        log->lux = q.value(1).toUInt();
        log->temperature = q.value(2).toReal();
        log->humidity = q.value(3).toReal();
        v->push_back(*log);
    }
    return v;
}

bool LocalDb::putSetting(QString key, QString value)
{

    QSqlQuery q;
    bool ret = q.exec(sqlInsertTbSettings.arg(key).arg(value));
    qDebug().noquote() << QString("%1 [insert ret=%2]: key='%3' value='%4'")
                              .arg(tbNameSettings)
                              .arg(ret)
                              .arg(key)
                              .arg(value);
    return ret;
}

QString LocalDb::getSetting(QString key)
{
    QSqlQuery q;
    bool ret = q.exec(QString("select value from %1 where key='%2'").arg(tbNameSettings).arg(key));
    q.next();
    QString value = q.value(0).toString();
    qDebug().noquote() << QString("%1 [select]: key='%2' value='%3'")
                              .arg(tbNameSettings)
                              .arg(key)
                              .arg(value);
    return value;
}

void LocalDb::test()
{
    connect("test.db");
    QSqlQuery q;
    q.exec(sqlDrop.arg(tbNameGzws));
    q.exec(sqlCreateTbGzws);
    putGzws(123212, 23, 45.6, 1610174085240);
    putGzws(23425, 45, 12.5, 1610174568932);
    //    QSqlQuery q;
    q.exec(QString("select * from %1").arg(tbNameGzws));
    while (q.next()) {
        qDebug().noquote() << QString("%1 lux %2C %3% %4")
                                  .arg(q.value(1).toInt())
                                  .arg(q.value(2).toReal())
                                  .arg(q.value(3).toReal())
                                  .arg(q.value(4).toLongLong());
    }
}
