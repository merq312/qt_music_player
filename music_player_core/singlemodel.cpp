#include "singlemodel.h"
#include <QPixmap>
#include <QMimeData>
#include <QDataStream>

using namespace std;

SingleModel::SingleModel(QObject *parent) :
    QAbstractListModel(parent),
    _db(DatabaseManager::instance()),
    _singles(vector<unique_ptr<Single>>())
{
}

void SingleModel::addSingle(Single &single)
{
    _db.singleDAO.addSingle(single);
}

void SingleModel::setAlbum(const QString &albumName)
{
    beginResetModel();
    loadSingles(albumName);
    endResetModel();
}

int SingleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _singles.size();
}

QVariant SingleModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index)) {
        return QVariant();
    }
    const Single& single = *_singles.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return single._title;

        case Roles::UrlRole:
            return single._fileUrl;

        case Roles::FilePathRole:
            return single._fileUrl.toLocalFile();

        case Roles::CoverArtRole:
            return *single._coverArtImage;

        default:
            return QVariant();
    }
}

Qt::ItemFlags SingleModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | defaultFlags;
    }
    else {
        return Qt::ItemIsDropEnabled | defaultFlags;
    }
}

QMimeData *SingleModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    for (const QModelIndex &index : indexes) {
        if (index.isValid()) {

            int id = _singles.at(index.row())->_id;
            QUrl filePath = _singles.at(index.row())->_fileUrl;
            QString title = _singles.at(index.row())->_title;
            QString albumName = _singles.at(index.row())->_albumName;
            QString albumArtist = _singles.at(index.row())->_albumArtist;
            QPixmap *coverArtImage = _singles.at(index.row())->_coverArtImage;

            stream << id << filePath << title << albumName << albumArtist << *coverArtImage;
        }
    }
    mimeData->setData("application/playlist.model", encodedData);
    return mimeData;
}

bool SingleModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row >= rowCount()
                || count < 0
                || (row + count) > rowCount()) {
        return false;
    }
    beginRemoveRows(parent, row, row + count - 1);

    int countLeft = count;
    while (countLeft--) {
        const Single& single = *_singles.at(row + countLeft);
        _db.singleDAO.removeSingle(single._id);
    }
    _singles.erase(_singles.begin() + row,
                   _singles.begin() + row + count);

    endRemoveRows();

    return true;
}

void SingleModel::loadSingles(const QString &albumName)
{
    _singles = _db.singleDAO.getSingles(albumName);
}

bool SingleModel::isIndexValid(const QModelIndex &index) const
{
    if (index.row() < 0 || index.row() > rowCount()
                        || !index.isValid()) {
        return false;
    }
    return true;
}
