#include "coverartdelegate.h"
#include "albummodel.h"

#include <QPainter>

const unsigned int BANNER_HEIGHT = 20;
const unsigned int BANNER_TEXT_COLOR = 0x000000;

CoverArtDelegate::CoverArtDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void CoverArtDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    painter->save();

    QPixmap pixmap = index.model()->data(index,
                                         Qt::DecorationRole).value<QPixmap>();
    painter->drawPixmap(option.rect.x(),
                        option.rect.y(),
                        pixmap);

    QRect bannerAlbumRect = QRect(option.rect.x(),
                                   option.rect.y() + option.rect.height() - 2*BANNER_HEIGHT,
                                   pixmap.width(),
                                   BANNER_HEIGHT);
    QRect bannerArtistRect = QRect(option.rect.x(),
                                  option.rect.y() + option.rect.height() - BANNER_HEIGHT,
                                  pixmap.width(),
                                  BANNER_HEIGHT);

    QString albumArtist = index.model()->data(index, AlbumModel::Roles::ArtistNameRole).toString();
    QString albumName = index.model()->data(index, Qt::DisplayRole).toString();

    if (albumName.length() > 30) {
        albumName.chop(albumName.length() - 30);
        albumName = albumName + "...";
    }

    painter->setPen(BANNER_TEXT_COLOR);

    QFont font = painter->font();
    font.setPixelSize(10);

    font.setBold(true);
    painter->setFont(font);
    painter->drawText(bannerAlbumRect, Qt::AlignCenter, albumName);

    font.setBold(false);
    painter->setFont(font);
    painter->drawText(bannerArtistRect, Qt::AlignCenter, albumArtist);

    if (option.state.testFlag(QStyle::State_Selected)) {
        QPen pen(option.palette.highlight().color());
        pen.setWidth(2);
        painter->setPen(pen);
        painter->drawRect(option.rect.marginsRemoved(QMargins(1 , 1, 1, 2*BANNER_HEIGHT + 1)));
    }

    painter->restore();
}

QSize CoverArtDelegate::sizeHint(const QStyleOptionViewItem &/*option*/,
                                 const QModelIndex &index) const
{
    const QPixmap &pixmap = index.model()->data(index, Qt::DecorationRole).value<QPixmap>();
    QSize size = pixmap.size();
    size.setHeight(size.height() + 2*BANNER_HEIGHT);

    return size;
}
