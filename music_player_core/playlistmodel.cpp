#include "playlistmodel.h"

#include <QPixmap>
#include <QMimeData>
#include <QDataStream>

#include <QDebug>
#include <algorithm>

using namespace std;

PlaylistModel::PlaylistModel(QObject *parent) :
    QAbstractListModel(parent),
    _playlist(vector<unique_ptr<Single>>())
{

}

void PlaylistModel::moveIndexUp(int row)
{
    auto iter = next(_playlist.begin(), row);
    auto iter_up = prev(iter);

    beginResetModel();
    iter_swap(iter, iter_up);
    endResetModel();
}

void PlaylistModel::moveIndexDown(int row)
{
    auto iter = next(_playlist.begin(), row);
    auto iter_down = next(iter);

    beginResetModel();
    iter_swap(iter, iter_down);
    endResetModel();

}

QModelIndex PlaylistModel::addSingle(Single &single)
{
    int rows = rowCount();
    beginInsertRows(QModelIndex(), rows, rows);
    unique_ptr<Single> newSingle(new Single(single));
    _playlist.push_back(move(newSingle));
    endInsertRows();

    return index(rows, 0);
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _playlist.size();
}

bool PlaylistModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > rowCount()
                || count < 0) {
        return false;
    }

    beginInsertRows(parent, row, row + count - 1);

    for (int i = 0; i < count; i++) {
        auto iter = next(_playlist.begin(), row + i);
        unique_ptr<Single> newSingle(new Single());
        _playlist.insert(iter, move(newSingle));
    }

    endInsertRows();

    emit changeCurrentPlaying(row, count);

    return true;
}

bool PlaylistModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row >= rowCount()
                || count < 0
                || (row + count) > rowCount()) {
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);

    _playlist.erase(_playlist.begin() + row,
                    _playlist.begin() + row + count);

    endRemoveRows();

    emit changeCurrentPlaying(row, -count);

    return true;
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index)) {
        return QVariant();
    }
    const Single &single = *_playlist.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return single._title;

        case Roles::UrlRole:
            return single._fileUrl;

        case Roles::FilePathRole:
            return single._fileUrl.toLocalFile();

        case Roles::CoverArtRole:
            return *single._coverArtImage;

        case Roles::ArtistNameRole:
            return single._albumArtist;

        default:
            return QVariant();
    }

}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!isIndexValid(index)) {
        return false;
    }
    if (role == Qt::EditRole) {
        _playlist[index.row()]->_title = value.toString();
        emit dataChanged(index, index);

        return true;
    }
    return false;
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if (index.isValid()) {
        return Qt::ItemIsDragEnabled /*| Qt::ItemIsDropEnabled*/ | defaultFlags;
    }
    else {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

Qt::DropActions PlaylistModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}


QStringList PlaylistModel::mimeTypes() const
{
    QStringList types;
    types << "application/playlist.model";

    return types;
}

bool PlaylistModel::canDropMimeData(const QMimeData *data,
                                    Qt::DropAction action,
                                    int row,
                                    int column,
                                    const QModelIndex &parent) const
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);

    if ((action != Qt::MoveAction && action != Qt::CopyAction) ||
         !data->hasFormat("application/playlist.model")) {

        return false;
    }
    return true;
}

QMimeData *PlaylistModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for (const QModelIndex &index : indexes) {
        if (index.isValid()) {
            int id = _playlist.at(index.row())->_id;
            QUrl filePath = _playlist.at(index.row())->_fileUrl;
            QString title = _playlist.at(index.row())->_title;
            QString albumName = _playlist.at(index.row())->_albumName;
            QString albumArtist = _playlist.at(index.row())->_albumArtist;
            QPixmap *coverArtImage = _playlist.at(index.row())->_coverArtImage;

            stream << id << filePath << title << albumName << albumArtist << *coverArtImage;
        }
    }
    mimeData->setData("application/playlist.model", encodedData);

    return mimeData;
}

bool PlaylistModel::dropMimeData(const QMimeData *data,
                                 Qt::DropAction action,
                                 int row,
                                 int column,
                                 const QModelIndex &parent)
{
    if (row == -1 && column == -1) {
        column = 0;
        row = rowCount();
    }

    if (!canDropMimeData(data, action, row, column, parent)) {
        return false;
    }

    if (action == Qt::IgnoreAction) {
        return true;
    }
    else if (action != Qt::MoveAction && action != Qt::CopyAction) {
        return false;
    }

    QByteArray encodedData = data->data("application/playlist.model");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    vector<unique_ptr<Single>> newSingles;

    while (!stream.atEnd()) {
        unique_ptr<Single> single(new Single);
        stream >> single->_id;
        stream >> single->_fileUrl;
        stream >> single->_title;
        stream >> single->_albumName;
        stream >> single->_albumArtist;

        QPixmap coverArtImage;
        stream >> coverArtImage;
        single->_coverArtImage = new QPixmap(coverArtImage);

        newSingles.push_back(move(single));
    }

    insertRows(row, newSingles.size(), QModelIndex());
    for (unsigned int i = 0; i < newSingles.size(); i++) {
        _playlist[row + i] = move(newSingles[i]);
    }

    return true;
}

bool PlaylistModel::isIndexValid(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() > rowCount()
                        || !index.isValid()) {
        return false;
    }
    return true;
}


