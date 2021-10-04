#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H

#include "music_player_core_global.h"
#include <QAbstractListModel>
#include <QPair>
#include <QString>

#include "databasemanager.h"

class QPixmap;

class MUSIC_PLAYER_CORE_EXPORT AlbumModel : public QAbstractListModel
{
    Q_OBJECT
        
public:

    enum Roles {
        ArtistNameRole = Qt::UserRole + 1,
    };

    AlbumModel(QObject *parent = 0);

    void refreshAlbumList();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    bool isIndexValid(const QModelIndex& index) const;

private:
    DatabaseManager &_db;
    QList<QPair<QString, QString>> _albumList;
    QList<QPixmap *> _coverArtList;
};

#endif // ALBUMMODEL_H
