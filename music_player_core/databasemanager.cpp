#include "databasemanager.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager singleton;
    return singleton;
}

DatabaseManager::~DatabaseManager()
{
    _database->close();
}

DatabaseManager::DatabaseManager(const QString &path) :
    _database(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))),
    singleDAO(*_database)
{
    _database->setDatabaseName(path);

    bool openStatus = _database->open();
    qDebug() << "Database connection: " << (openStatus ? "OK" : "Error");

    singleDAO.init();
}
