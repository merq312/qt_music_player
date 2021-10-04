#include "playerviewer.h"
#include "ui_playerviewer.h"

#include <QInputDialog>
#include <QFileDialog>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "singlemodel.h"
#include "albummodel.h"
#include "playlistmodel.h"
#include "metadatareader.h"
#include "coverartdelegate.h"

#include <QListWidget>
#include <QListView>
#include <memory>

QString getTimeString(int dura) {

    QString mins{};
    QString secs{};
    dura = dura/1000;
    if (dura/60 < 10) {
        mins = "0" + QString::number(dura/60);
    } else {
        mins = QString::number(dura/60);
    }
    if (dura%60 < 10) {
        secs = "0" + QString::number(dura%60);
    } else {
        secs = QString::number(dura%60);
    }
    return QString(mins + ":" + secs);
}

PlayerViewer::PlayerViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerViewer),
    timer(new QTimer(this)),
    _SingleModel(nullptr),
    _SingleSelectionModel(nullptr)
{
    ui->setupUi(this);
    timer->setInterval(REFRESH_INTERVAL);

    ui->playBtn->setIcon(QIcon(":/icons/play"));
    ui->stopBtn->setIcon(QIcon(":/icons/stop"));
    ui->nextBtn->setIcon(QIcon(":/icons/next"));
    ui->prevBtn->setIcon(QIcon(":/icons/prev"));
    ui->tinyCoverArt->setPixmap(QPixmap(":/icons/defaultCoverArt50"));

    ui->stopBtn->setEnabled(false);
    ui->playbackSlider->setEnabled(false);

    ui->albumListView->setSpacing(1);
    ui->albumListView->setResizeMode(QListView::Adjust);
    ui->albumListView->setFlow(QListView::LeftToRight);
    ui->albumListView->setWrapping(true);
    ui->albumListView->setItemDelegate(new CoverArtDelegate(this));
//    ui->albumListView->setViewMode(QListView::IconMode);

//    **********************************************************************************

    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView->setDragEnabled(true);
    ui->listView->setDefaultDropAction(Qt::DropAction::CopyAction);

    ui->playlistView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->playlistView->setDragEnabled(true);
    ui->playlistView->viewport()->setAcceptDrops(true);
    ui->playlistView->setDropIndicatorShown(true);
    ui->playlistView->setDefaultDropAction(Qt::DropAction::MoveAction);

//    **********************************************************************************


    connect(ui->addSingleBtn, &QPushButton::clicked,
            this, &PlayerViewer::addSingles);
    connect(ui->removeSingleBtn, &QPushButton::clicked,
            this, &PlayerViewer::removeSingles);

    connect(ui->playlistView, &QListView::doubleClicked,
            [this] {
                QVariant currentSelected = getSelection();
                playbackStopped();

                if (!currentSelected.isNull()) {
                    setCurrentLabel();
                    emit playSingle(currentSelected.toUrl());
                }
    });

    connect(ui->listView, &QListView::doubleClicked,
            [this](QModelIndex index) {
                std::unique_ptr<Single> single(new Single);
                *single = *_SingleModel->_singles.at(index.row());

                _PlaylistModel->beginResetModel();
                _PlaylistModel->_playlist.push_back(std::move(single));
                _PlaylistModel->endResetModel();
    });

    connect(ui->playBtn, &QPushButton::clicked,
            [this] {
                if (isPlaying == true) {
                    emit pauseSingle();
                }
                else {
                    QVariant currentSelected = getSelection();
                    if (!currentSelected.isNull()) {
                        setCurrentLabel();
                        emit playSingle(currentSelected.toUrl());
                    }
                }
    });

    connect(ui->stopBtn, &QPushButton::clicked,
            [this] {
                playbackStopped();
                emit stopSingle();
    });

    connect(ui->nextBtn, &QPushButton::clicked,
            [this] {
                QVariant nextSelected = getNextSelection();
                playbackStopped();

                if (!nextSelected.isNull()) {
                    emit playSingle(nextSelected.toUrl());
                }
                else {
                    emit stopSingle();
                }
    });

    connect(ui->prevBtn, &QPushButton::clicked,
            [this] {
                QVariant prevSelected = getPrevSelection();
                playbackStopped();

                if (!prevSelected.isNull()) {
                    emit playSingle(prevSelected.toUrl());
                }
                else {
                    emit stopSingle();
                }
    });

    connect(timer, &QTimer::timeout,
            [this] {
                int pos = ui->playbackSlider->value();
                pos += 1000;

                if (pos >= ui->playbackSlider->maximum()) {
                    timer->stop();
                    ui->playbackSlider->setValue(0);
                    ui->duraCurrent->setText(getTimeString(0));
                    ui->playbackSlider->setEnabled(false);

                    QVariant nextSelected = getNextSelection();
                    if (!nextSelected.isNull()) {
                        emit playSingle(nextSelected.toUrl());
                    } else {
                        playbackStopped();
                    }
                }
                else {
                    ui->playbackSlider->setValue(pos);
                    ui->duraCurrent->setText(getTimeString(pos));
                }
    });

    connect(ui->playbackSlider, &QSlider::sliderMoved,
            [this] {
                int pos = ui->playbackSlider->value();
                ui->duraCurrent->setText(getTimeString(pos));
    });

    connect(ui->playbackSlider, &QSlider::sliderReleased,
            [this] {
                emit playbackPosChanged(ui->playbackSlider->value());
    });

    connect(ui->clearPlaylistBtn, &QPushButton::clicked,
            [this] {
                _PlaylistModel->beginResetModel();
                if (isPlaying) {
                    emit stopSingle();
                    playbackStopped();
                }
                _PlaylistModel->removeRows(0, _PlaylistModel->rowCount(), QModelIndex());
                currentPlaying = QModelIndex();
                _PlaylistModel->endResetModel();
    });

    connect(ui->removeFromPlaylistBtn, &QPushButton::clicked,
            [this] {
                QModelIndexList currentIndexList = _PlaylistSelectionModel->selectedIndexes();
                int row{99999};
                int count{0};

                for (auto index : currentIndexList) {
                    if (index.isValid()) {
                        if (row > index.row()) {
                            row = index.row();
                        }
                        if (index == currentPlaying) {
                            emit stopSingle();
                            playbackStopped();
                            currentPlaying = QModelIndex();
                        }
                    }
                    count++;
                }

                _PlaylistModel->beginResetModel();
//                _PlaylistModel->beginRemoveRows(QModelIndex(), row, row + count - 1);
                _PlaylistModel->removeRows(row, count, QModelIndex());
//                _PlaylistModel->endRemoveRows();
                _PlaylistModel->endResetModel();

    });

    connect(ui->MoveUpBtn, &QPushButton::clicked,
            [this] {
                int row = _PlaylistSelectionModel->currentIndex().row();
                if (row > 0) {
                    _PlaylistModel->moveIndexUp(row);
                    _PlaylistSelectionModel->setCurrentIndex(_PlaylistModel->index(row - 1, 0), QItemSelectionModel::ClearAndSelect);
                }
    });

    connect(ui->MoveDownBtn, &QPushButton::clicked,
            [this] {
                int row = _PlaylistSelectionModel->currentIndex().row();
                if (row != -1 && row < _PlaylistModel->rowCount() - 1) {
                    _PlaylistModel->moveIndexDown(row);
                    _PlaylistSelectionModel->setCurrentIndex(_PlaylistModel->index(row + 1, 0), QItemSelectionModel::ClearAndSelect);
                }
    });
}

PlayerViewer::~PlayerViewer()
{
    delete ui;
    delete _SingleModel;
    delete _SingleSelectionModel;
    delete _AlbumModel;
    delete _AlbumSelectionModel;
    delete _PlaylistModel;
    delete _PlaylistSelectionModel;
    delete timer;
}

void PlayerViewer::playbackStopped()
{
    if (timer->isActive()) {
        timer->stop();
    }
    ui->stopBtn->setEnabled(false);
    ui->playBtn->setIcon(QIcon(":/icons/play"));

    ui->duraCurrent->setText("00:00");
    ui->playbackSlider->setValue(0);
    ui->playbackSlider->setEnabled(false);

    isPlaying = false;
}

void PlayerViewer::playbackStarted(qint64 dura)
{
    if (!timer->isActive()) {
        ui->stopBtn->setEnabled(true);
        ui->playBtn->setIcon(QIcon(":/icons/pause"));

        ui->playbackSlider->setMaximum(dura);
        ui->playbackSlider->setEnabled(true);

        ui->duraTotal->setText(getTimeString(dura));

        timer->start();

        isPlaying = true;
    }
}

void PlayerViewer::playbackPaused(bool state)
{
    if (state == true) {
        timer->start();
        ui->playBtn->setIcon(QIcon(":/icons/pause"));
    }
    else {
        timer->stop();
        ui->playBtn->setIcon(QIcon(":/icons/play"));
    }
}

void PlayerViewer::setSingleModel(SingleModel *model)
{
    _SingleModel = model;
    ui->listView->setModel(model);
}

void PlayerViewer::setSingleSelectionModel(QItemSelectionModel *selectionModel)
{
    _SingleSelectionModel = selectionModel;
    ui->listView->setSelectionModel(selectionModel);
}

void PlayerViewer::setAlbumModel(AlbumModel *model)
{
    _AlbumModel = model;
    ui->albumListView->setModel(model);
}

void PlayerViewer::setAlbumSelectionModel(QItemSelectionModel *selectionModel)
{
    _AlbumSelectionModel = selectionModel;
    ui->albumListView->setSelectionModel(selectionModel);

    connect(_AlbumSelectionModel, &QItemSelectionModel::selectionChanged,
            [this](const QItemSelection &selected) {
                if (selected.isEmpty()) {
                    return;
                }
                loadAlbum(selected.indexes().first());
//                if (selected.indexes().first() == currentPlayingAlbum) {
//                    _SingleSelectionModel->setCurrentIndex(currentPlaying, QItemSelectionModel::ClearAndSelect);
//                }
    });
}

void PlayerViewer::setPlaylistModel(PlaylistModel *model)
{
    _PlaylistModel = model;
    ui->playlistView->setModel(model);

    connect(_PlaylistModel, &PlaylistModel::changeCurrentPlaying,
            [this](int row, int count){
                int currentRow = currentPlaying.row();

                if (currentRow == -1) {
                    return;
                }

                if (row < currentRow) {
                    currentRow += count;
                    currentPlaying  = _PlaylistModel->index(currentRow, 0);
                }
    });
}

void PlayerViewer::setPlaylistSelectionModel(QItemSelectionModel *selectionModel)
{
    _PlaylistSelectionModel = selectionModel;
    ui->playlistView->setSelectionModel(selectionModel);
}

QVariant PlayerViewer::getSelection()
{
    if (!_PlaylistSelectionModel->currentIndex().isValid()) {
        _PlaylistSelectionModel->setCurrentIndex(_PlaylistModel->index(0, 0), QItemSelectionModel::ClearAndSelect);
    }
    currentPlaying = _PlaylistSelectionModel->currentIndex();
//    currentPlayingAlbum = _AlbumSelectionModel->currentIndex();
    QVariant currentFile = _PlaylistModel->data(currentPlaying, PlaylistModel::Roles::UrlRole);

    return currentFile;
}

QVariant PlayerViewer::getNextSelection()
{
    if (!currentPlaying.isValid()) {
        return QVariant();
    }

//    _AlbumSelectionModel->setCurrentIndex(currentPlayingAlbum, QItemSelectionModel::ClearAndSelect);
    int row = currentPlaying.row();
    QModelIndex nextModelIndex = _PlaylistModel->index(row + 1, 0);

    if (nextModelIndex.isValid()) {
        _PlaylistSelectionModel->setCurrentIndex(nextModelIndex, QItemSelectionModel::ClearAndSelect);
        currentPlaying = nextModelIndex;
        setCurrentLabel();
        QVariant currentFile = _PlaylistModel->data(currentPlaying, PlaylistModel::Roles::UrlRole);
        return currentFile;
    }
    else {
        return QVariant();
    }
}

QVariant PlayerViewer::getPrevSelection()
{
    if (!currentPlaying.isValid()) {
        return QVariant();
    }

//    _AlbumSelectionModel->setCurrentIndex(currentPlayingAlbum, QItemSelectionModel::ClearAndSelect);
    int row = currentPlaying.row();
    QModelIndex prevModelIndex = _PlaylistModel->index(row - 1, 0);

    if (prevModelIndex.isValid()) {
        _PlaylistSelectionModel->setCurrentIndex(prevModelIndex, QItemSelectionModel::ClearAndSelect);
        currentPlaying = prevModelIndex;
        setCurrentLabel();
        QVariant currentFile = _PlaylistModel->data(currentPlaying, PlaylistModel::Roles::UrlRole);
        return currentFile;
    }
    else {
        return QVariant();
    }
}

void PlayerViewer::setCurrentLabel()
{
    ui->currentLabel->setText(currentPlaying.data(Qt::DisplayRole).toString());
    ui->currentArtistLabel->setText(currentPlaying.data(PlaylistModel::Roles::ArtistNameRole).toString());
    QPixmap pixmap = currentPlaying.data(PlaylistModel::Roles::CoverArtRole).value<QPixmap>();
    if (!pixmap.isNull()) {
        ui->tinyCoverArt->setPixmap(pixmap.scaled(50, 50));
    }
    else {
        ui->tinyCoverArt->setPixmap(QPixmap(":/icons/defaultCoverArt50"));
    }
}

//void PlayerViewer::addSingles()
//{
//    QStringList filenames = QFileDialog::getOpenFileNames(this,
//                                                          "Add Songs",
//                                                          QDir::homePath(),
//                                                          "Audio files  (*.mp3)");
//    if (!filenames.isEmpty()) {

//        MetadataReader metadatareader(this);
//        QEventLoop loop;
//        QModelIndex lastModelIndex;
//        QTimer loopTimeout;
//        loopTimeout.setSingleShot(true);

//        connect(&metadatareader, &MetadataReader::readMetadataComplete,
//                &loop, &QEventLoop::quit);
//        connect(&loopTimeout, &QTimer::timeout,
//                &loop, &QEventLoop::quit);

//        for (int i = 0; i < filenames.size(); i++) {
//            QString filename = filenames.at(i);
//            Single single(filename);
//            metadatareader.setSingle(&single);
//            metadatareader.readMetadata();
//            loopTimeout.start(5000);
//            loop.exec();

//            if (single.isMetaSet == true) {
//                lastModelIndex = _SingleModel->addSingle(single);
//            } else {
//                qDebug() << "Error reading metadata on" << filename;
//            }
//        }
//        _AlbumModel->refreshAlbumList();
//        ui->listView->setCurrentIndex(lastModelIndex);
//    }
//}

void addSinglesRecursive(QString dirpath, QStringList &audiofiles) {

    if (!dirpath.isEmpty()) {
        QDir dir = dirpath;
        QFileInfoList filenames = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

        for (auto filename : filenames) {
            QFileInfo fileinfo(filename);
            if (fileinfo.isDir()) {
                addSinglesRecursive(filename.filePath(), audiofiles);
            }
            else if (fileinfo.isFile()) {
                if (fileinfo.suffix() == "mp3") {
                    audiofiles.push_back(filename.filePath());
                }
            }
        }

    }
}
void PlayerViewer::addSingles()
{
    QString dirpath = QFileDialog::getExistingDirectory(this,
                                                        "Add directories",
                                                        QDir::homePath());

    QStringList audiofiles;
    addSinglesRecursive(dirpath, audiofiles);

    if (!audiofiles.isEmpty()) {

        MetadataReader metadatareader(this);
        QEventLoop loop;
        // QModelIndex lastModelIndex;
        QTimer loopTimeout;
        loopTimeout.setSingleShot(true);

        connect(&metadatareader, &MetadataReader::readMetadataComplete,
                &loop, &QEventLoop::quit);
        connect(&loopTimeout, &QTimer::timeout,
                &loop, &QEventLoop::quit);

        for (int i = 0; i < audiofiles.size(); i++) {
            QString filename = audiofiles.at(i);
            Single single(filename);
            metadatareader.setSingle(&single);
            metadatareader.readMetadata();
            loopTimeout.start(5000);
            loop.exec();

            if (single.isMetaSet == true) {
                // lastModelIndex = _SingleModel->addSingle(single);
                _SingleModel->addSingle(single);
            } else if (single.isMetaBad == true) {
                qDebug() << "Metadata bad, trying to read again";
                i--;
            }
            else {
                qDebug() << "(Timeout) Error reading metadata on" << filename;
            }
        }
        _AlbumModel->refreshAlbumList();
//        ui->listView->setCurrentIndex(lastModelIndex);
    }
}

void PlayerViewer::removeSingles()
{
    if (_SingleSelectionModel->selectedIndexes().isEmpty()) {
        return;
    }
    emit stopSingle();
    int row = _SingleSelectionModel->currentIndex().row();
    _SingleModel->removeRow(row);
    _AlbumModel->refreshAlbumList();

    if (_SingleModel->rowCount() != 0) {
        QModelIndex nextModelIndex = _SingleModel->index(row, 0);
        if (nextModelIndex.isValid()) {
            _SingleSelectionModel->select(nextModelIndex, QItemSelectionModel::ClearAndSelect);
            _SingleSelectionModel->setCurrentIndex(nextModelIndex, QItemSelectionModel::ClearAndSelect);

            return;
        }
        QModelIndex prevModelIndex = _SingleModel->index(row - 1, 0);
        if (prevModelIndex.isValid()) {
            _SingleSelectionModel->select(prevModelIndex, QItemSelectionModel::ClearAndSelect);
            _SingleSelectionModel->setCurrentIndex(prevModelIndex, QItemSelectionModel::ClearAndSelect);

            return;
        }
    }
}

void PlayerViewer::loadAlbum(const QModelIndex &albumIndex)
{
    _SingleModel->setAlbum(_AlbumModel->data(albumIndex).toString());
}
