#ifndef DATABASE_H
#define DATABASE_H

#include "QSqlDatabase"

class Database
{

public:
    static bool connect();
    static bool initializeSchema();
};

#endif // DATABASE_H
