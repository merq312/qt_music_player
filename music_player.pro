TEMPLATE = subdirs

SUBDIRS += \
    music_player_core \
    music_player_desktop

music_player_desktop.depends = music_player_core
