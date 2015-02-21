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

