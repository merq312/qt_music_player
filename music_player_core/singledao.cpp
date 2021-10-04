#include "singledao.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QPixmap>
#include <QBuffer>

#include "single.h"

using namespace std;

SingleDAO::SingleDAO(QSqlDatabase &database) :
    _database(database)
{
}

void SingleDAO::init() const
{
    if (!_database.tables().contains("singles")) {
        QSqlQuery query(_database);
        query.exec(QString("CREATE TABLE singles (id INTEGER PRIMARY KEY AUTOINCREMENT, ") + 
                   "url TEXT, title TEXT, albumName TEXT, albumArtist TEXT, year INTEGER, coverArt BLOB)");
    }
}

void SingleDAO::addSingle(Single &single) const
{
    QSqlQuery query(_database);
    query.prepare(QString("INSERT INTO singles (url, title, albumName, albumArtist, year, coverArt) ") +
                  "VALUES (:url, :title, :albumName, :albumArtist, :year, :coverArt)");

    query.bindValue(":url", single._fileUrl);
    query.bindValue(":title", single._title);
    query.bindValue(":albumName", single._albumName);
    query.bindValue(":albumArtist", single._albumArtist);
    query.bindValue(":year", single._year);

    QByteArray inByteArray;
    QBuffer inBuffer(&inByteArray);
    inBuffer.open(QIODevice::WriteOnly);
    single._coverArtImage->save(&inBuffer, "PNG");

    query.bindValue(":coverArt", inByteArray);

    query.exec();
    single._id = query.lastInsertId().toInt();
}

void SingleDAO::removeSingle(int id) const
{
    QSqlQuery query(_database);
    query.prepare("DELETE FROM singles WHERE id = (:id)");
    query.bindValue(":id", id);
    query.exec();
}

void SingleDAO::removeAlbum(QPair<QString, QString> albumInfoPair) const
{
    QSqlQuery query(_database);
    query.prepare("DELETE FROM singles WHERE albumName = (:albumName) AND albumArtist = (:albumArtist");
    query.bindValue(":albumName", albumInfoPair.first);
    query.bindValue(":albumArtist", albumInfoPair.second);
    query.exec();
}

std::vector<std::unique_ptr<Single>> SingleDAO::getSingles(const QString &albumName) const
{
    QSqlQuery query(_database);
    query.prepare("SELECT * FROM singles WHERE albumName = (:albumName)");
    query.bindValue(":albumName", albumName);
    query.exec();

    vector<unique_ptr<Single>> list;

    while(query.next()) {
        unique_ptr<Single> single(new Single());

        single->_id = query.value("id").toInt();
        single->_fileUrl = query.value("url").toString();
        single->_title = query.value("title").toString();
        single->_albumName = query.value("albumName").toString();
        single->_albumArtist = query.value("albumArtist").toString();
        single->_year = query.value("year").toInt();

        QByteArray outByteArray = query.value("coverArt").toByteArray();
        QPixmap *outPixmap = new QPixmap();
        outPixmap->loadFromData(outByteArray);

        single->_coverArtImage = outPixmap;

        list.push_back(move(single));
    }

    return list;
}

QList<QPair<QString, QString>> SingleDAO::getAlbums() const
{
    QSqlQuery query(_database);
    query.prepare("SELECT DISTINCT albumName, albumArtist FROM singles GROUP BY albumName ORDER BY albumArtist ASC, year ASC");
    query.exec();

    QList<QPair<QString, QString>> albumList;
    while(query.next()) {
        QString albumName = query.value("albumName").toString();
        QString albumArtist = query.value("albumArtist").toString();

        albumList.push_back(QPair<QString, QString>(albumName, albumArtist));
    }
    return albumList;
}

QList<QPixmap *> SingleDAO::getAlbumArt() const
{
    QSqlQuery query(_database);
    query.prepare("SELECT DISTINCT albumName, coverArt FROM singles GROUP BY albumName ORDER BY albumArtist ASC, year ASC");
    query.exec();

    QList<QPixmap *> coverArt;
    while(query.next()) {
        QByteArray outByteArray = query.value("coverArt").toByteArray();
        QPixmap *outPixmap = new QPixmap();
        outPixmap->loadFromData(outByteArray);

        coverArt.push_back(outPixmap);
    }
    return coverArt;
}
