#ifndef DATABASE_H
#define DATABASE_H

#include "QSqlDatabase"

class Database
{

public:
    static bool connect();
    static bool initializeSchema();
    static QSqlDatabase getConnection();

private:
    static QSqlDatabase db;
};

#endif // DATABASE_H
