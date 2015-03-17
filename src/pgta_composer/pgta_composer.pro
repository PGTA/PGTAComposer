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
            widgets \
            multimedia

TARGET =    "PGTAComposer"
DESTDIR = 	$$PWD/../../bin/

FORMS +=    forms/mainwindow.ui \ 
            forms/PGTATrackView.ui \
    forms/PGTAPropertiesView.ui
HEADERS +=  ./FileUtils.h \
            ./FlatbufferTrackLoader.h \
            ./mainwindow.h \
            ./PGTATrackItem.h \
            ./PGTATrackTreeModel.h \
            ./FlatbufferTrackWriter.h \
            ./PGTAProject.h \
            ./PGTALibraryModel.h \
            ./PGTALibraryItem.h \
            ./PGTAConstants.h \
            ./PGTATreeView.h \
            ./PGTATrackView.h \
            ./PGTADockable.h \
            ./PGTAPropertiesView.h
SOURCES +=  ./FileUtils.cpp \
            ./FlatbufferTrackLoader.cpp \
            ./main.cpp \
            ./mainwindow.cpp \
            ./PGTATrackItem.cpp \
            ./PGTATrackTreeModel.cpp \
            ./FlatbufferTrackWriter.cpp \
            ./PGTAProject.cpp \
            ./PGTALibraryModel.cpp \
            ./PGTALibraryItem.cpp \
            ./PGTATreeView.cpp \
            ./PGTATrackView.cpp \
            ./PGTADockable.cpp \
            ./PGTAPropertiesView.cpp

INCLUDEPATH +=  $$PWD/../external/PGTA/src/external/flatbuffers/include/
DEPENDPATH +=  $$PWD/../external/PGTA/src/external/flatbuffers/include/
INCLUDEPATH += $$PWD/../external/PGTA/src/PGTA/include/
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

linux {
    LIBS += $$PWD/../qmake/gmake/libPGTALib.so
    PRE_TARGETDEPS += $$PWD/../qmake/gmake/libPGTALib.so
    LIBS += $$PWD/../qmake/gmake/libSDL2.so
    PRE_TARGETDEPS += $$PWD/../qmake/gmake/libSDL2.so
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

