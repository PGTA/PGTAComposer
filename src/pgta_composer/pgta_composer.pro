#-------------------------------------------------
#
# Project created by QtCreator 2014-08-08T00:30:54
#
#-------------------------------------------------

QT       += core gui \
            widgets

TARGET = pgta_composer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Track.pb.cc

QMAKE_CXXFLAGS += -std=c++0x

LIBS += -lprotobuf

HEADERS  += mainwindow.h \
    Track.pb.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    pgta_composer.pro.user
