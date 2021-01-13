#include <QSqlDatabase>
#include <QString>
#include <string>

#ifndef LOCALDB_H
#define LOCALDB_H

struct gzws_log {
    quint64 ctime;
    quint16 lux;
    qreal temperature;
    qreal humidity;
};

class LocalDb {
public:
    LocalDb() = default;
    /* after connect(), all functions should be available */
    bool connect(QString dbName);
    bool isConnected() { return this->connected; }
    /* Function about gzws */
    bool putGzws(quint16 lux, qreal temperature, qreal humidity, quint64 ctime);
    gzws_log getGzws(); // default: last hour

    /* Settings */
    bool putSetting(QString key, QString value);
    QString getSetting(QString key);

    void test();

private:
    bool connected = false;
    QString dbName;
    QSqlDatabase db;
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
