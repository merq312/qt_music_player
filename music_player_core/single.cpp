#include "single.h"
#include <QPixmap>

Single::Single(const QString &filePath) :
    Single(QUrl::fromLocalFile(filePath))
{
}

Single::Single(const QUrl &fileUrl) :
    _id(-1),
    _fileUrl(fileUrl),
    _title("untitled"),
    _albumName("unknown album"),
    _albumArtist("unknown artist"),
    _year(0),
    _coverArtImage(nullptr)
{
}

Single::~Single()
{
    delete _coverArtImage;
}
