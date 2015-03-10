#-------------------------------------------------
#
# PGTA COMPOSER SUITE CONFIG
#
#-------------------------------------------------

!include( common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE = subdirs
SUBDIRS = $$PWD/../pgta_composer

