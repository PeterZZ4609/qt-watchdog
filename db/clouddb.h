#include <iostream>
#include <string>

#include <QSqlDatabase>

#ifndef CLOUDDB_H
#define CLOUDDB_H

class CloudDb {

public:
    CloudDb() = default;
    // 读取配置文件的内容
    void initConfig(std::string host, unsigned short port, std::string user, std::string pass);
    // 建立数据库连接，传入数据库名
    bool connect(std::string dbName);

public:
    std::string dbName = ""; // 数据库名
    QSqlDatabase db;
};

#endif // CLOUDDB_H
