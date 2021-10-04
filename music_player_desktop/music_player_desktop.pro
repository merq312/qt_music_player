QT       += core gui sql multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    coverartdelegate.cpp \
    main.cpp \
    mainwindow.cpp \
    playerviewer.cpp

HEADERS += \
    coverartdelegate.h \
    mainwindow.h \
    playerviewer.h

FORMS += \
    mainwindow.ui \
    playerviewer.ui

RESOURCES += \
    resource.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../music_player_core/release/ -lmusic_player_core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../music_player_core/debug/ -lmusic_player_core
else:unix: LIBS += -L$$OUT_PWD/../music_player_core/ -lmusic_player_core

INCLUDEPATH += $$PWD/../music_player_core
DEPENDPATH += $$PWD/../music_player_core
