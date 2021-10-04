#include "metadatareader.h"

#include <QMediaPlayer>
#include <QImage>
#include <QPixmap>

#include "single.h"

MetadataReader::MetadataReader(QObject *parent) :
    QObject(parent),
    mediaDevice(new QMediaPlayer),
    prevTitle("")
{
    connect(mediaDevice, &QMediaPlayer::metaDataAvailableChanged,
            [this] {
                if (mediaDevice->isMetaDataAvailable() == true) {

                    if (prevTitle == mediaDevice->metaData("Title").toString()) {
                        single->isMetaBad = true;
                        emit readMetadataComplete();
                    }
                    else {
                        single->_title = mediaDevice->metaData("Title").toString();
                        prevTitle = single->_title;

                        if (!mediaDevice->metaData("AlbumArtist").isNull()) {
                            single->_albumArtist = mediaDevice->metaData("AlbumArtist").toString();
                        } else {
                            single->_albumArtist = mediaDevice->metaData("ContributingArtist").toString();
                        }

                        single->_albumName = mediaDevice->metaData("AlbumTitle").toString();
                        single->_year = mediaDevice->metaData("Year").toInt();

                        QImage img(mediaDevice->metaData("CoverArtImage").value<QImage>());
                        QPixmap *pixmap = new QPixmap(QPixmap::fromImage(img));
                        *pixmap = pixmap->scaled(150, 150);

                        single->_coverArtImage = pixmap;

                        single->isMetaSet = true;

                        emit readMetadataComplete();
                    }
                }
    });
}

void MetadataReader::setSingle(Single *single)
{
    this->single = single;
}

void MetadataReader::readMetadata()
{
    mediaDevice->setMedia(single->_fileUrl);
}
