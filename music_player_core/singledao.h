#ifndef SINGLEDAO_H
#define SINGLEDAO_H

#include "music_player_core_global.h"
#include <memory>
#include <vector>
#include <QString>
#include <QPair>
#include <QList>

class QSqlDatabase;
class Single;
class QPixmap;

class MUSIC_PLAYER_CORE_EXPORT SingleDAO
{
public:
    SingleDAO(QSqlDatabase &database);
    void init() const;

    void addSingle(Single &single) const;
    void removeSingle(int id) const;
    void removeAlbum(QPair<QString, QString> albumInfoPair) const;

    std::vector<std::unique_ptr<Single>> getSingles(const QString &albumName) const;
    QList<QPair<QString, QString>> getAlbums() const;
    QList<QPixmap *> getAlbumArt() const;

private:
    QSqlDatabase &_database;
};

#endif // SINGLEDAO_H
