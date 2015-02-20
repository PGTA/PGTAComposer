#-------------------------------------------------
#
# PGTA COMPOSER QT PROJECT CONFIG
#
#-------------------------------------------------

QT +=       core \
            gui \
            widgets

TARGET =    PGTAComposer
unix:OBJECTS_DIR = $$PWD/gmake/obj/
unix:MOC_DIR = $$PWD/gmake/moc/
unix:UI_DIR = $$PWD/gmake/ui/
DESTDIR = 	$$PWD/../../bin/


TEMPLATE =  app
CONFIG +=   c++11

FORMS +=    $$PWD/../pgta_composer/forms/mainwindow.ui
HEADERS +=  $$PWD/../pgta_composer/*.h
SOURCES +=  $$PWD/../pgta_composer/*.cpp

QMAKE_MAC_SDK = macosx10.9
