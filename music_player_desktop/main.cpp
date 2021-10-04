#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("");
    w.show();
    return a.exec();
}

/* ****************************************************************************************************************
 * TODO:
 *
 * &1 - more testing required
 * &2 - replace current hacky system with taglib
 * &3 - lots of messy code here.
 *      DONE fix single member access levels.
 *      DONE should album art be stored as a QVariant so that Model/View data function can be used? (no)
 *
 * - DONE Play/Pause functionality
 * - DONE&1 Continuous play
 * - DONE blank "music" icon when no single is loaded
 * - Volume control - implement custom painted QWidget
 * - Mute option
 * - Playback visualizer (use OpenGL?)
 *
 * Albums Refactor&3
 * - DONE&2 Read metadata from files
 * - DONE Search folders recursively for .mp3 files
 * - DONE Needs to update album list when files added/removed
 * - DONE Select album to load singles. Currently loads all singles.
 * - DONE SQL query must provide albums sorted by artist->year
 * - DONE Loading albums should reset model
 * - Loading albums while PLAYING music breaks model
 *      The best solution to this may be to implement the currently playing list as a playlist
 *      Therefore any changes to the model will not affect the current playlist.
 *
 * Playlist Refactor
 * - DONE Implement PlaylistModel class
 * - DONE Empty by default. Entries are loaded onto model from SingleModel and AlbumModel
 *
 * - REMOVE Implementations for Move Up and Move Down buttons!!!
 *
 * Core Refactor
 * - DONE Separate and implement core functionality as shared library!
 *
 * - FIX Add and Remove button functionality --> what should they even do???
 * - FIX Button behavior when music comes to natural stop
 * - FIX Crashes occur intermittently, usually after playlistmodel is modified
 * -
 *
 * ****************************************************************************************************************/
