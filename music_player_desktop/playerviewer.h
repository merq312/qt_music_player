#ifndef PLAYERVIEWER_H
#define PLAYERVIEWER_H

#include <QWidget>
#include <QUrl>
#include <QItemSelectionModel>

namespace Ui {
class PlayerViewer;
}

class SingleModel;
class AlbumModel;
class PlaylistModel;
class QTimer;

class PlayerViewer : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerViewer(QWidget *parent = nullptr);
    ~PlayerViewer();

    void playbackStopped();
    void playbackStarted(qint64 dura);
    void playbackPaused(bool state);

    void setSingleModel(SingleModel *model);
    void setSingleSelectionModel(QItemSelectionModel *selectionModel);

    void setAlbumModel(AlbumModel *model);
    void setAlbumSelectionModel(QItemSelectionModel *selectionModel);

    void setPlaylistModel(PlaylistModel *model);
    void setPlaylistSelectionModel(QItemSelectionModel *selectionModel);

    QVariant getSelection();
    QVariant getNextSelection();
    QVariant getPrevSelection();

private:
    void setCurrentLabel();
    void loadAlbum(const QModelIndex& albumIndex);

signals:
    void playSingle(QUrl);
    void pauseSingle();
    void stopSingle();
    void nextSingle(QUrl);
    void prevSingle(QUrl);

    void playbackPosChanged(qint64);

private slots:
    void addSingles();
    void removeSingles();

private:
    Ui::PlayerViewer *ui;

    static const int REFRESH_INTERVAL{1000};
    QTimer *timer;

    SingleModel *_SingleModel;
    QItemSelectionModel *_SingleSelectionModel;

    AlbumModel *_AlbumModel;
    QItemSelectionModel *_AlbumSelectionModel;

    PlaylistModel *_PlaylistModel;
    QItemSelectionModel *_PlaylistSelectionModel;

    QModelIndex currentPlaying;
//    QModelIndex currentPlayingAlbum;
    bool isPlaying{false};
};

#endif // PLAYERVIEWER_H
