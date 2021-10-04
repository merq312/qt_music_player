#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include "music_player_core_global.h"
#include <QAbstractListModel>
#include <vector>
#include <memory>

#include "single.h"

class MUSIC_PLAYER_CORE_EXPORT PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

public:

    friend class PlayerViewer;

    enum Roles {
        UrlRole = Qt::UserRole + 1,
        FilePathRole,
        CoverArtRole,
        ArtistNameRole,
    };

    PlaylistModel(QObject *parent = nullptr);

    void moveIndexUp(int row);
    void moveIndexDown(int row);

    QModelIndex addSingle(Single &single);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;

    QStringList mimeTypes() const override;

    bool canDropMimeData(const QMimeData *data,
                         Qt::DropAction action,
                         int row,
                         int column,
                         const QModelIndex &parent) const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex &parent) override;

signals:
    void changeCurrentPlaying(int row, int count);

private:
    bool isIndexValid(const QModelIndex &index) const;

private:
    std::vector<std::unique_ptr<Single>> _playlist;
};

#endif // PLAYLISTMODEL_H
