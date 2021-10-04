#ifndef COVERARTDELEGATE_H
#define COVERARTDELEGATE_H

#include <QStyledItemDelegate>

class CoverArtDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    CoverArtDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter,
              const QStyleOptionViewItem &option,
              const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};

#endif // COVERARTDELEGATE_H
