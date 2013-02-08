#-------------------------------------------------
#
# Project created by QtCreator 2013-02-08T16:04:30
#
#-------------------------------------------------

QT       += xml

QT       -= gui

TARGET = GPX
TEMPLATE = lib

DEFINES += GPX_LIBRARY

SOURCES += GPX.cpp \
    GPXXMLHandler.cpp \
    GPXPoint.cpp

HEADERS += GPX.h\
        GPX_global.h \
    GPXXMLHandler.h \
    GPXPoint.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
