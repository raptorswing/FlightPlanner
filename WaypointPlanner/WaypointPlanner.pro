#-------------------------------------------------
#
# Project created by QtCreator 2013-03-15T11:22:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WaypointPlanner
TEMPLATE = app


SOURCES += main.cpp\
        WaypointPlannerMainWindow.cpp \
    WaypointMapView.cpp \
    Waypoint.cpp \
    WaysetManager.cpp \
    DubinsLineObject.cpp

HEADERS  += WaypointPlannerMainWindow.h \
    WaypointMapView.h \
    Waypoint.h \
    WaysetManager.h \
    DubinsLineObject.h

FORMS    += WaypointPlannerMainWindow.ui

RESOURCES += \
    WaypointPlannerResources.qrc


#Linkage for MapGraphics
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/debug/ -lMapGraphics
else:unix: LIBS += -L$$OUT_PWD/../MapGraphics/ -lMapGraphics

INCLUDEPATH += $$PWD/../MapGraphics
DEPENDPATH += $$PWD/../MapGraphics

#Linkage for GPX
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GPX/release/ -lGPX
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GPX/debug/ -lGPX
else:unix: LIBS += -L$$OUT_PWD/../GPX/ -lGPX

INCLUDEPATH += $$PWD/../GPX
DEPENDPATH += $$PWD/../GPX

#Linkage for Dubins
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Dubins/release/ -lDubins
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Dubins/debug/ -lDubins
else:unix: LIBS += -L$$OUT_PWD/../Dubins/ -lDubins

INCLUDEPATH += $$PWD/../Dubins
DEPENDPATH += $$PWD/../Dubins

#Linkage for PlanningCommon
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PlanningCommon/release/ -lPlanningCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PlanningCommon/debug/ -lPlanningCommon
else:unix: LIBS += -L$$OUT_PWD/../PlanningCommon/ -lPlanningCommon

INCLUDEPATH += $$PWD/../PlanningCommon
DEPENDPATH += $$PWD/../PlanningCommon

#Linkage for QVectorND
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/release/ -lQVectorND
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/debug/ -lQVectorND
else:unix: LIBS += -L$$OUT_PWD/../QVectorND/ -lQVectorND

INCLUDEPATH += $$PWD/../QVectorND
DEPENDPATH += $$PWD/../QVectorND
