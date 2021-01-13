#include "clouddb.h"

void CloudDb::initConfig(std::string host, unsigned short port, std::string user, std::string pass)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(QString(host.c_str()));
    db.setPort(port);
    db.setUserName(QString(user.c_str()));
    db.setPassword(QString(pass.c_str()));
}

bool CloudDb::connect(std::string dbName)
{
    db.setDatabaseName(QString(dbName.c_str()));
    return db.open();
}
