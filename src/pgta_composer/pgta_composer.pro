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
            trackengine.cpp

CONFIG +=   c++11

HEADERS +=  mainwindow.h \
            Track.pb.h \
    trackengine.h

FORMS +=    mainwindow.ui

OTHER_FILES +=  pgta_composer.pro.user

QMAKE_MAC_SDK = macosx10.9

LIBS +=     -L/usr/local/lib/ -lprotobuf
INCLUDEPATH += /usr/local/include

DEPENDPATH +=   /usr/local/include
PRE_TARGETDEPS +=   /usr/local/lib/libprotobuf.a