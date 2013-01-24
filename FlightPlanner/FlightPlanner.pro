#-------------------------------------------------
#
# Project created by QtCreator 2012-10-18T11:35:27
#
#-------------------------------------------------

QT       += core gui

TARGET = FlightPlanner
TEMPLATE = app


SOURCES += main.cpp\
    FlightTasks/FlightTask.cpp \
    gui/MainWindow.cpp \
    gui/PaletteWidget.cpp \
    gui/PlanningControlWidget.cpp \
    FlightPlanner.cpp \
    PlanningProblem.cpp \
    UAVOrientation.cpp \
    FlightTaskArea.cpp \
    FlightTasks/FlyThroughTask.cpp \
    ProblemViewAdapter.cpp \
    GreedyFlightPlanner.cpp \
    gui/StartPosMapObject.cpp \
    gui/FlightTaskAreaMapObject.cpp \
    gui/FlightTaskAreaObjectEditWidget.cpp \
    gui/FlightTaskAreaListModel.cpp \
    GreedyPlanningNode.cpp \
    Fitness.cpp \
    FlightTasks/NoFlyFlightTask.cpp \
    DummyPlanner.cpp \
    HierarchicalPlanner/HierarchicalPlanner.cpp \
    QVectorND.cpp \
    HierarchicalPlanner/SubFlightPlanner.cpp \
    HierarchicalPlanner/SubFlightNode.cpp \
    FlightTasks/CoverageTask.cpp

HEADERS  += \
    FlightTasks/FlightTask.h \
    gui/MainWindow.h \
    gui/PaletteWidget.h \
    gui/PlanningControlWidget.h \
    FlightPlanner.h \
    PlanningProblem.h \
    UAVOrientation.h \
    FlightTaskArea.h \
    FlightTasks/FlyThroughTask.h \
    ProblemViewAdapter.h \
    GreedyFlightPlanner.h \
    gui/StartPosMapObject.h \
    gui/FlightTaskAreaMapObject.h \
    gui/FlightTaskAreaObjectEditWidget.h \
    gui/FlightTaskAreaListModel.h \
    GreedyPlanningNode.h \
    Fitness.h \
    FlightTasks/NoFlyFlightTask.h \
    DummyPlanner.h \
    HierarchicalPlanner/HierarchicalPlanner.h \
    QVectorND.h \
    HierarchicalPlanner/SubFlightPlanner.h \
    HierarchicalPlanner/SubFlightNode.h \
    FlightTasks/CoverageTask.h

FORMS    += gui/MainWindow.ui \
    gui/PaletteWidget.ui \
    gui/PlanningControlWidget.ui \
    gui/FlightTaskAreaObjectEditWidget.ui

RESOURCES += \
    resources.qrc


#Linkage for MapGraphics library.
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/debug/ -lMapGraphics
else:symbian: LIBS += -lMapGraphics
else:unix: LIBS += -L$$OUT_PWD/../MapGraphics/ -lMapGraphics

INCLUDEPATH += $$PWD/../MapGraphics
DEPENDPATH += $$PWD/../MapGraphics
