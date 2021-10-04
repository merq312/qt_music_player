#ifndef SINGLEMODEL_H
#define SINGLEMODEL_H

#include "music_player_core_global.h"
#include <QAbstractListModel>
#include <vector>
#include <memory>

#include "single.h"
#include "databasemanager.h"

class MUSIC_PLAYER_CORE_EXPORT SingleModel : public QAbstractListModel
{
    Q_OBJECT

public:

    friend class PlayerViewer;

    enum Roles {
        UrlRole = Qt::UserRole + 1,
        FilePathRole,
        CoverArtRole
    };

    SingleModel(QObject* parent = nullptr);

    void addSingle(Single& single);
    void setAlbum(const QString &albumName);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    bool removeRows(int row, int count, const QModelIndex& parent) override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

private:
    void loadSingles(const QString &albumName);
    bool isIndexValid(const QModelIndex& index) const;

private:
    DatabaseManager &_db;
    std::vector<std::unique_ptr<Single>> _singles;
};

#endif // SINGLEMODEL_H
