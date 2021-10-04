#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <memory>
#include <QString>

#include "singledao.h"

class QSqlDatabase;

const QString DATABASE_FILENAME = "music.db";

class DatabaseManager
{
public:
    static DatabaseManager &instance();
    ~DatabaseManager();

protected:
    DatabaseManager(const QString &path = DATABASE_FILENAME);
    DatabaseManager operator=(const DatabaseManager &rhs);

private:
    std::unique_ptr<QSqlDatabase> _database;

public:
    const SingleDAO singleDAO;
};

#endif // DATABASEMANAGER_H
