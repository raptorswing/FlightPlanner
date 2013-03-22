#-------------------------------------------------
#
# Project created by QtCreator 2013-03-21T16:51:22
#
#-------------------------------------------------

QT       += gui

TARGET = Dubins
TEMPLATE = lib

DEFINES += DUBINS_LIBRARY

SOURCES += \
    Dubins.cpp \
    dubinGuts/dubinsSolver.cpp

HEADERS +=\
        Dubins_global.h \
    Dubins.h \
    dubinGuts/dubinsSolver.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
