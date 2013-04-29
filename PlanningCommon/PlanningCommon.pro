#-------------------------------------------------
#
# Project created by QtCreator 2013-03-22T11:40:39
#
#-------------------------------------------------

QT       += gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QMAKE_CXXFLAGS += -std=c++0x

TARGET = PlanningCommon
TEMPLATE = lib

DEFINES += PLANNINGCOMMON_LIBRARY

SOURCES += \
    UAVParameters.cpp \
    UAVOrientation.cpp \
    Serializable.cpp \
    PlanningProblem.cpp \
    FlightTaskArea.cpp \
    FlightPlanner.cpp \
    Fitness.cpp \
    FlightTasks/TimingConstraint.cpp \
    FlightTasks/SamplingTask.cpp \
    FlightTasks/NoFlyFlightTask.cpp \
    FlightTasks/FlyThroughTask.cpp \
    FlightTasks/FlightTask.cpp \
    FlightTasks/CoverageTask.cpp \
    GreedyPlanner/GreedyPlanningNode.cpp \
    GreedyPlanner/GreedyFlightPlanner.cpp \
    HierarchicalPlanner/IntermediatePlanner.cpp \
    HierarchicalPlanner/HierarchicalPlanner.cpp \
    HierarchicalPlanner/AstarPRMIntermediatePlanner/AstarPRMIntermediatePlanner.cpp \
    HierarchicalPlanner/DubinsIntermediate/DubinsIntermediatePlanner.cpp \
    HierarchicalPlanner/PhonyIntermediatePlanner/PhonyIntermediatePlanner.cpp \
    HierarchicalPlanner/RRTIntermediatePlanner/RRTIntermediatePlanner.cpp \
    HierarchicalPlanner/RRTIntermediatePlanner/RRTDistanceMetric.cpp \
    HierarchicalPlanner/SubFlightPlanner/SubFlightPlanner.cpp \
    HierarchicalPlanner/SubFlightPlanner/SubFlightNode.cpp \
    Exporters/GPXExporter.cpp \
    Exporters/Exporter.cpp \
    Importers/importer.cpp \
    Importers/GPXImporter.cpp \
    gui/CommonFileHandling.cpp \
    MapObjects/Waypoint.cpp \
    MapObjects/DubinsLineObject.cpp \
    WaysetDisplayManager.cpp \
    Wayset.cpp \
    gui/CommonWindowHandling.cpp \
    UAVPose.cpp \
    SimulatedFlier.cpp \
    Exporters/BinaryExporter.cpp \
    Importers/BinaryImporter.cpp \
    gui/ChatWidget.cpp \
    UserStudyChatHandler.cpp \
    HierarchicalPlanner/SmartIntermediatePlanner.cpp

HEADERS +=\
        PlanningCommon_global.h \
    UAVParameters.h \
    UAVOrientation.h \
    Serializable.h \
    PlanningProblem.h \
    FlightTaskArea.h \
    FlightPlanner.h \
    Fitness.h \
    FlightTasks/TimingConstraint.h \
    FlightTasks/SamplingTask.h \
    FlightTasks/NoFlyFlightTask.h \
    FlightTasks/FlyThroughTask.h \
    FlightTasks/FlightTask.h \
    FlightTasks/CoverageTask.h \
    GreedyPlanner/GreedyPlanningNode.h \
    GreedyPlanner/GreedyFlightPlanner.h \
    HierarchicalPlanner/IntermediatePlanner.h \
    HierarchicalPlanner/HierarchicalPlanner.h \
    HierarchicalPlanner/AstarPRMIntermediatePlanner/AstarPRMIntermediatePlanner.h \
    HierarchicalPlanner/DubinsIntermediate/DubinsIntermediatePlanner.h \
    HierarchicalPlanner/PhonyIntermediatePlanner/PhonyIntermediatePlanner.h \
    HierarchicalPlanner/RRTIntermediatePlanner/RRTIntermediatePlanner.h \
    HierarchicalPlanner/RRTIntermediatePlanner/RRTDistanceMetric.h \
    HierarchicalPlanner/SubFlightPlanner/SubFlightPlanner.h \
    HierarchicalPlanner/SubFlightPlanner/SubFlightNode.h \
    Exporters/GPXExporter.h \
    Exporters/Exporter.h \
    Importers/Importer.h \
    Importers/GPXImporter.h \
    gui/CommonFileHandling.h \
    MapObjects/Waypoint.h \
    MapObjects/DubinsLineObject.h \
    WaysetDisplayManager.h \
    Wayset.h \
    gui/CommonWindowHandling.h \
    UAVPose.h \
    SimulatedFlier.h \
    Exporters/BinaryExporter.h \
    Importers/BinaryImporter.h \
    gui/ChatWidget.h \
    UserStudyChatHandler.h \
    HierarchicalPlanner/SmartIntermediatePlanner.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

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

#Linkage for Dubins
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Dubins/release/ -lDubins
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Dubins/debug/ -lDubins
else:unix: LIBS += -L$$OUT_PWD/../Dubins/ -lDubins

INCLUDEPATH += $$PWD/../Dubins
DEPENDPATH += $$PWD/../Dubins

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

FORMS += \
    gui/ChatWidget.ui
