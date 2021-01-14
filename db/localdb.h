#include <QDebug>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>

#ifndef LOCALDB_H
#define LOCALDB_H

struct gzws_log {
    quint64 ctime;
    quint16 lux;
    qreal temperature;
    qreal humidity;
};
/**
 * @brief The LocalDb class
 * @author PeterZhang
 * @date unknown
 * sqlite工具类，用于：
 * 1. 将传感器数据写入本地数据库、以及读取历史数据
 * 2. 读取、写入用户配置
 */
class LocalDb {
public:
    LocalDb() = default;
    LocalDb(QString _dbName);
    /* after connect(), all functions should be available */
    bool connect(QString _dbName);
    bool isConnected() { return this->connected; }
    /* db info */
    QString driverName() { return db.driverName(); }
    QString dbName() { return _dbName; }
    qint64 dbSize() { return QFile(_dbName).size(); }
    QStringList tables() { return db.tables(); }

    /* gzws */
    bool putGzws(quint32 lux, qreal temperature, qreal humidity, qint64 ctime);
    gzws_log getGzws(); // default: last hour
    QVector<gzws_log>* getGzws(qint64 from, qint64 to);

    /* settings */
    bool putSetting(QString key, QString value);
    QString getSetting(QString key);

    void test();

public:
    QSqlDatabase db;

private:
    bool connected = false;
    QString _dbName;
    // 表名
    QString tbNameGzws = "log_gzws";
    QString tbNameSettings = "settings";
    // 建表语句
    QString sqlDrop = QString("drop table if exists %1");
    QString sqlCreateTbGzws
        = QString("create table if not exists %1 \
            ( id    integer primary key autoincrement, \
              lux   integer check ( lux >= 0 and lux <= 200000 ), \
              tmp   real, \
              rh    real, \
              ctime datetime )")
              .arg(tbNameGzws);
    QString sqlCreateTbSettings
        = QString("create table if not exists %1 \
              ( id    integer primary key autoincrement, \
                key   varchar(64) not null unique, \
                value nvarchar(256) );")
              .arg(tbNameSettings);
    // 插入语句
    QString sqlInsertTbGzws
        = QString("insert into %1(lux, tmp, rh, ctime) values (%2,%3,%4,%5)").arg(tbNameGzws);
    QString sqlInsertTbSettings
        = QString("insert into %1(key, value) values ('%2', '%3')").arg(tbNameSettings);
};

#endif // LOCALDB_H
