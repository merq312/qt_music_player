#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QItemSelection>
#include <QDebug>
#include <QMediaPlayer>

#include "playerviewer.h"
#include "singlemodel.h"
#include "albummodel.h"
#include "playlistmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    player(new QMediaPlayer)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":icons/play"));

    player->setVolume(50);

    SingleModel* singleModel = new SingleModel(this);
    QItemSelectionModel* singleSelectionModel = new QItemSelectionModel(singleModel, this);

    AlbumModel* albumModel = new AlbumModel(this);
    QItemSelectionModel* albumSelectionModel = new QItemSelectionModel(albumModel, this);

    PlaylistModel* playlistModel = new PlaylistModel(this);
    QItemSelectionModel* playlistSelectionModel = new QItemSelectionModel(playlistModel, this);

    ui->playerViewer->setSingleModel(singleModel);
    ui->playerViewer->setSingleSelectionModel(singleSelectionModel);

    ui->playerViewer->setAlbumModel(albumModel);
    ui->playerViewer->setAlbumSelectionModel(albumSelectionModel);

    ui->playerViewer->setPlaylistModel(playlistModel);
    ui->playerViewer->setPlaylistSelectionModel(playlistSelectionModel);


//    connect(player, &QMediaPlayer::metaDataAvailableChanged,
//            [this] {
//                QStringList meta = player->availableMetaData();

//                for (auto m : meta) {
//                    qDebug() << m << player->metaData(m);
//                }
//    });

    connect(player, &QMediaPlayer::durationChanged,
            [this](qint64 dura) {
                if (dura != 0) {
                    ui->playerViewer->playbackStarted(dura);
                }
    });

    connect(ui->playerViewer, &PlayerViewer::playSingle,
            [this](QUrl currentSelected) {
                    player->setMedia(currentSelected);
                    player->play();
    });

    connect(ui->playerViewer, &PlayerViewer::pauseSingle,
            [this] {
                if (player->state() == QMediaPlayer::PausedState) {
                    player->play();
                    ui->playerViewer->playbackPaused(true);

                } else if (player->state() == QMediaPlayer::PlayingState) {
                    player->pause();
                    ui->playerViewer->playbackPaused(false);
                }
    });

    connect(ui->playerViewer, &PlayerViewer::stopSingle,
            [this] {
                player->stop();
    });

    connect(ui->playerViewer, &PlayerViewer::nextSingle,
            [this](QUrl nextSelected) {
                player->setMedia(nextSelected);
                player->play();
    });

    connect(ui->playerViewer, &PlayerViewer::prevSingle,
            [this](QUrl prevSelected) {
                player->setMedia(prevSelected);
                player->play();
    });

    connect(ui->playerViewer, &PlayerViewer::playbackPosChanged,
            [this](qint64 value) {
                player->setPosition(value);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete player;
}

