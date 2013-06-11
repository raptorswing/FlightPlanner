#-------------------------------------------------
#
# Project created by QtCreator 2013-06-10T17:40:35
#
#-------------------------------------------------

QT       += core gui

TARGET = BenchmarkBlaster
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    BenchmarkDriver.cpp

#Linkage for PlanningCommon
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PlanningCommon/release/ -lPlanningCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PlanningCommon/debug/ -lPlanningCommon
else:unix: LIBS += -L$$OUT_PWD/../PlanningCommon/ -lPlanningCommon

INCLUDEPATH += $$PWD/../PlanningCommon
DEPENDPATH += $$PWD/../PlanningCommon

HEADERS += \
    BenchmarkDriver.h

#Linkage for MapGraphics
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/debug/ -lMapGraphics
else:unix: LIBS += -L$$OUT_PWD/../MapGraphics/ -lMapGraphics

INCLUDEPATH += $$PWD/../MapGraphics
DEPENDPATH += $$PWD/../MapGraphics

#Linkage for Dubins
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Dubins/release/ -lDubins
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Dubins/debug/ -lDubins
else:unix: LIBS += -L$$OUT_PWD/../Dubins/ -lDubins

INCLUDEPATH += $$PWD/../Dubins
DEPENDPATH += $$PWD/../Dubins

#Linkage for GPX
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GPX/release/ -lGPX
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GPX/debug/ -lGPX
else:unix: LIBS += -L$$OUT_PWD/../GPX/ -lGPX

INCLUDEPATH += $$PWD/../GPX
DEPENDPATH += $$PWD/../GPX

#Linkage for QVectorND
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/release/ -lQVectorND
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/debug/ -lQVectorND
else:unix: LIBS += -L$$OUT_PWD/../QVectorND/ -lQVectorND

INCLUDEPATH += $$PWD/../QVectorND
DEPENDPATH += $$PWD/../QVectorND

#Linkage for QKDTree
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QKDTree/release/ -lQKDTree
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QKDTree/debug/ -lQKDTree
else:unix: LIBS += -L$$OUT_PWD/../QKDTree/ -lQKDTree

INCLUDEPATH += $$PWD/../QKDTree
DEPENDPATH += $$PWD/../QKDTree
