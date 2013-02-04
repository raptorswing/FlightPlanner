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
    GreedyPlanner/GreedyFlightPlanner.cpp \
    MapObjects/StartPosMapObject.cpp \
    MapObjects/FlightTaskAreaMapObject.cpp \
    gui/FlightTaskAreaObjectEditWidget.cpp \
    gui/FlightTaskAreaListModel.cpp \
    GreedyPlanner/GreedyPlanningNode.cpp \
    Fitness.cpp \
    FlightTasks/NoFlyFlightTask.cpp \
    HierarchicalPlanner/HierarchicalPlanner.cpp \
    HierarchicalPlanner/SubFlightPlanner/SubFlightPlanner.cpp \
    HierarchicalPlanner/SubFlightPlanner/SubFlightNode.cpp \
    FlightTasks/CoverageTask.cpp \
    HierarchicalPlanner/IntermediatePlanner.cpp \
    HierarchicalPlanner/RRTIntermediatePlanner/RRTIntermediatePlanner.cpp \
    HierarchicalPlanner/RRTIntermediatePlanner/RRTDistanceMetric.cpp \
    HierarchicalPlanner/AstarPRMIntermediatePlanner/AstarPRMIntermediatePlanner.cpp \
    HierarchicalPlanner/PhonyIntermediatePlanner/PhonyIntermediatePlanner.cpp \
    gui/FlightTaskDelegate.cpp \
    gui/FlightTaskRowEditor.cpp

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
    GreedyPlanner/GreedyFlightPlanner.h \
    MapObjects/StartPosMapObject.h \
    MapObjects/FlightTaskAreaMapObject.h \
    gui/FlightTaskAreaObjectEditWidget.h \
    gui/FlightTaskAreaListModel.h \
    GreedyPlanner/GreedyPlanningNode.h \
    Fitness.h \
    FlightTasks/NoFlyFlightTask.h \
    HierarchicalPlanner/HierarchicalPlanner.h \
    HierarchicalPlanner/SubFlightPlanner/SubFlightPlanner.h \
    HierarchicalPlanner/SubFlightPlanner/SubFlightNode.h \
    FlightTasks/CoverageTask.h \
    HierarchicalPlanner/IntermediatePlanner.h \
    HierarchicalPlanner/RRTIntermediatePlanner/RRTIntermediatePlanner.h \
    HierarchicalPlanner/RRTIntermediatePlanner/RRTDistanceMetric.h \
    HierarchicalPlanner/AstarPRMIntermediatePlanner/AstarPRMIntermediatePlanner.h \
    HierarchicalPlanner/PhonyIntermediatePlanner/PhonyIntermediatePlanner.h \
    gui/FlightTaskDelegate.h \
    gui/FlightTaskRowEditor.h

FORMS    += gui/MainWindow.ui \
    gui/PaletteWidget.ui \
    gui/PlanningControlWidget.ui \
    gui/FlightTaskAreaObjectEditWidget.ui \
    gui/FlightTaskRowEditor.ui

RESOURCES += \
    resources.qrc


#Linkage for MapGraphics library.
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/release/ -lMapGraphics
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MapGraphics/debug/ -lMapGraphics
else:symbian: LIBS += -lMapGraphics
else:unix: LIBS += -L$$OUT_PWD/../MapGraphics/ -lMapGraphics

INCLUDEPATH += $$PWD/../MapGraphics
DEPENDPATH += $$PWD/../MapGraphics

#Linkage for QVectorND library.
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/release/ -lQVectorND
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QVectorND/debug/ -lQVectorND
else:unix: LIBS += -L$$OUT_PWD/../QVectorND/ -lQVectorND

INCLUDEPATH += $$PWD/../QVectorND
DEPENDPATH += $$PWD/../QVectorND

#Linkage for QKDTree library.
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QKDTree/release/ -lQKDTree
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QKDTree/debug/ -lQKDTree
else:unix: LIBS += -L$$OUT_PWD/../QKDTree/ -lQKDTree

INCLUDEPATH += $$PWD/../QKDTree
DEPENDPATH += $$PWD/../QKDTree
