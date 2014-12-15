#-------------------------------------------------
#
# PGTA COMPOSER QT PROJECT CONFIG
#
#-------------------------------------------------

QT +=       core gui \
            widgets

TARGET =    pgta_composer

TEMPLATE =  app

SOURCES +=  main.cpp \
            mainwindow.cpp \
            Track.pb.cc \
    tracktablemodel.cpp \
    TrackItem.cpp \
    TrackTreeModel.cpp \
    EngineTrack.cpp

CONFIG +=   c++11

HEADERS +=  mainwindow.h \
            Track.pb.h \
    tracktablemodel.h \
    TrackItem.h \
    TrackTreeModel.h \
    EngineTrack.h

FORMS +=    mainwindow.ui

OTHER_FILES +=  pgta_composer.pro.user

QMAKE_MAC_SDK = macosx10.9

LIBS +=     -L/usr/local/lib/ -lprotobuf
INCLUDEPATH += /usr/local/include

DEPENDPATH +=   /usr/local/include
PRE_TARGETDEPS +=   /usr/local/lib/libprotobuf.a
