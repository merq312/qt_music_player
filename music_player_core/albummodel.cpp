#include "albummodel.h"
#include <QPixmap>

using namespace std;

AlbumModel::AlbumModel(QObject *parent) :
    QAbstractListModel(parent),
    _db(DatabaseManager::instance()),
    _albumList(_db.singleDAO.getAlbums()),
    _coverArtList(_db.singleDAO.getAlbumArt())
{
}

void AlbumModel::refreshAlbumList()
{
    QList<QPair<QString, QString>> newList = _db.singleDAO.getAlbums();

    if (newList.size() != _albumList.size()) {
        QList<QPixmap *> newArt = _db.singleDAO.getAlbumArt();

        beginResetModel();
        _albumList = newList;
        _coverArtList = newArt;
        endResetModel();
    }
}

int AlbumModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _albumList.size();

}

QVariant AlbumModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index)) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole: {
            return _albumList.at(index.row()).first;
        }
        case Qt::DecorationRole: {
            QPixmap coverArt = *_coverArtList.at(index.row());
            if (!coverArt.isNull()) {
                return coverArt;
            } else {
                QPixmap defaultPixmap (":/icons/defaultCoverArt150");
                return defaultPixmap;
            }
        }
        case Roles::ArtistNameRole: {
            return _albumList.at(index.row()).second;
        }
        default:
            return QVariant();
    }
}

bool AlbumModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row >= rowCount()
                || count < 0
                || (row + count) > rowCount()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);

    int countLeft = count;
    while (countLeft--) {
        QPair<QString, QString> albumInfoPair = _albumList.at(row + countLeft);
        _db.singleDAO.removeAlbum(albumInfoPair);
    }
    _albumList.erase(_albumList.begin() + row,
                     _albumList.begin() + row + count);

    endRemoveRows();

    return true;
}

bool AlbumModel::isIndexValid(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() > rowCount()
                        || !index.isValid()) {
        return false;
    }
    return true;
}
