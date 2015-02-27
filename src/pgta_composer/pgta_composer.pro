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

TARGET =    PGTAComposer
DESTDIR = 	$$PWD/../../bin/

FORMS +=    forms/mainwindow.ui
HEADERS +=  ./*.h
SOURCES +=  ./*.cpp

INCLUDEPATH +=  $$PWD/../external/PGTA/src/external/flatbuffers/include/
INCLUDEPATH += $$PWD/../external/PGTA/src/PGTA/public/
DEPENDPATH +=  $$PWD/../external/PGTA/src/external/flatbuffers/include/

win32 {
    LIBS += $$PWD/../qmake/vs2013/_FlatBuffers.lib
    PRE_TARGETDEPS += $$PWD/../qmake/vs2013/_FlatBuffers.lib
}

unix {
    LIBS += $$PWD/../qmake/gmake/lib_FlatBuffers.a
    PRE_TARGETDEPS += $$PWD/../qmake/gmake/lib_FlatBuffers.a
}
