#ifndef METADATAREADER_H
#define METADATAREADER_H

#include "music_player_core_global.h"
#include <QObject>

class QMediaPlayer;
class Single;

class MUSIC_PLAYER_CORE_EXPORT MetadataReader : public QObject
{
    Q_OBJECT
public:
    explicit MetadataReader(QObject *parent = nullptr);

    void setSingle(Single *single);
    void readMetadata();

signals:
    void readMetadataComplete();

private:
    QMediaPlayer *mediaDevice;
    Single *single;

    QString prevTitle;

};

#endif // METADATAREADER_H
