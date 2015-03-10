#-------------------------------------------------
#
# PGTA COMPOSER QT PROJECT CONFIG
#
#-------------------------------------------------

!include( $$PWD/../qmake/common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE =  app

QT +=       core \
            gui \
            widgets

TARGET =    "PGTAComposer"
DESTDIR = 	$$PWD/../../bin/

FORMS +=    forms/mainwindow.ui
HEADERS +=  ./FileUtils.h \
            ./FlatbufferTrackLoader.h \
            ./mainwindow.h \
            ./TrackItem.h \
            ./TrackTreeModel.h \
            ./FlatbufferTrackWriter.h
SOURCES +=  ./FileUtils.cpp \
            ./FlatbufferTrackLoader.cpp \
            ./main.cpp \
            ./mainwindow.cpp \
            ./TrackItem.cpp \
            ./TrackTreeModel.cpp \
            ./FlatbufferTrackWriter.cpp

INCLUDEPATH +=  $$PWD/../external/PGTA/src/external/flatbuffers/include/
DEPENDPATH +=  $$PWD/../external/PGTA/src/external/flatbuffers/include/
INCLUDEPATH += $$PWD/../external/PGTA/src/PGTA/public/
INCLUDEPATH += $$PWD/../external/PGTA/src/tests/PGTATestCommon/
DEPENDPATH += $$PWD/../external/PGTA/src/tests/PGTATestCommon/


win32 {
    LIBS += $$PWD/../qmake/vs2013/_FlatBuffers.lib
    PRE_TARGETDEPS += $$PWD/../qmake/vs2013/_FlatBuffers.lib
}

unix {
    LIBS += $$PWD/../qmake/gmake/lib_FlatBuffers.a
    PRE_TARGETDEPS += $$PWD/../qmake/gmake/lib_FlatBuffers.a
    LIBS += $$PWD/../qmake/gmake/libPGTATestCommon_x64.a
    PRE_TARGETDEPS += $$PWD/../qmake/gmake/libPGTATestCommon_x64.a
}

macx {
    LIBS += $$PWD/../qmake/gmake/libPGTALib.dylib
    PRE_TARGETDEPS += $$PWD/../qmake/gmake/libPGTALib.dylib
    LIBS += $$PWD/../qmake/gmake/libSDL2.dylib
    PRE_TARGETDEPS += $$PWD/../qmake/gmake/libSDL2.dylib
}

RESOURCES += \
    stylesheet.qrc \
    icons.qrc \
    images.qrc

macx:ICON = $${PWD}/icons/icon.icns

