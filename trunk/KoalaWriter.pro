# -------------------------------------------------
# Project created by QtCreator 2010-07-28T14:32:51
# -------------------------------------------------
TARGET = KoalaWriter
TEMPLATE = app
VERSION = 1.0
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/editor.cpp
HEADERS += src/mainwindow.h \
    src/editor.h 
RESOURCES += src/data.qrc
QT += phonon
ICON = KoalaIcon.icns
APP_MUSIC_FILES.files = Music/Pitx_-_Little_Star.mp3 \
    Music/mika_-_Fall_to_pieces_-_Silence.mp3 \
    Music/rocavaco_-_Harp_Variations.mp3
APP_MUSIC_FILES.path = Contents/Resources/Music
APP_BACKGROUND_FILES.files = Background/bg1.jpg \
    Background/bg2.jpg \
    Background/bg3.jpg \
    Background/pict0307.jpg
APP_BACKGROUND_FILES.path = Contents/Resources/Background
APP_CLICK_FILES.files = Click/406__TicTacShutUp__click_1_d.wav \
    Click/96640__CGEffex__Sci_Fi_Beep_03.wav \
    Click/66220__PercussionFiend__bamboo1.wav
APP_CLICK_FILES.path = Contents/Resources/Click
QMAKE_BUNDLE_DATA += APP_MUSIC_FILES APP_BACKGROUND_FILES APP_CLICK_FILES

