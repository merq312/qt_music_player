#ifndef SINGLE_H
#define SINGLE_H

#include "music_player_core_global.h"
#include <QUrl>
#include <QString>
class QPixmap;

class MUSIC_PLAYER_CORE_EXPORT Single
{
public:
    Single(const QString &filePath = "");
    Single(const QUrl &fileUrl);

    ~Single();

    void readMetadata();

    bool isMetaSet{false};
    bool isMetaBad{false};

    friend class SingleDAO;
    friend class SingleModel;
    friend class PlaylistModel;
    friend class MetadataReader;

private:
    int _id;
    QUrl _fileUrl;
    QString _title;
    QString _albumName;
    QString _albumArtist;
    int _year;
    QPixmap *_coverArtImage;
};

#endif // SINGLE_H
