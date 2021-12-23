#-------------------------------------------------
#
# Project created by QtCreator 2012-10-18T11:35:27
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlightPlanner
TEMPLATE = app


SOURCES += main.cpp\
    FlightTasks/FlightTask.cpp \
    Importers/Importer.cpp \
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
    gui/FlightTaskAreaEditor/FlightTaskAreaObjectEditWidget.cpp \
    gui/FlightTaskAreaEditor/FlightTaskAreaListModel.cpp \
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
    gui/FlightTaskAreaEditor/FlightTaskDelegate.cpp \
    gui/FlightTaskAreaEditor/FlightTaskRowEditor.cpp \
    gui/UAVParametersWidget.cpp \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintSliders.cpp \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintEditor.cpp \
    FlightTasks/TimingConstraint.cpp \
    gui/FlightTaskEditors/FlightTaskEditorFactory.cpp \
    gui/FlightTaskEditors/CoverageTaskEditor.cpp \
    UAVParameters.cpp \
    Exporters/Exporter.cpp \
    Exporters/GPXExporter.cpp \
    FlightTasks/SamplingTask.cpp \
    HierarchicalPlanner/DubinsIntermediate/DubinsIntermediatePlanner.cpp \
    gui/FlightTaskEditors/SubWidgets/TaskNameEditor.cpp \
    gui/FlightTaskEditors/FlightTaskEditor.cpp \
    gui/FlightTaskEditors/SubWidgets/CoverageTaskEditorWidgets.cpp \
    gui/FlightTaskEditors/SubWidgets/SamplingTaskEditorWidgets.cpp \
    gui/FlightTaskEditors/SamplingTaskEditor.cpp \
    gui/FlightTaskEditors/SubWidgets/DependencyConstraintEditor.cpp \
    gui/FlightTaskEditors/SubWidgets/DependencyRow.cpp \
    gui/FlightTaskEditors/FlyThroughTaskEditor.cpp \
    Serializable.cpp \
    Importers/GPXImporter.cpp

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
    gui/FlightTaskAreaEditor/FlightTaskAreaObjectEditWidget.h \
    gui/FlightTaskAreaEditor/FlightTaskAreaListModel.h \
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
    gui/FlightTaskAreaEditor/FlightTaskDelegate.h \
    gui/FlightTaskAreaEditor/FlightTaskRowEditor.h \
    gui/UAVParametersWidget.h \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintSliders.h \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintEditor.h \
    FlightTasks/TimingConstraint.h \
    gui/FlightTaskEditors/FlightTaskEditorFactory.h \
    gui/FlightTaskEditors/CoverageTaskEditor.h \
    UAVParameters.h \
    Exporters/Exporter.h \
    Exporters/GPXExporter.h \
    FlightTasks/SamplingTask.h \
    HierarchicalPlanner/DubinsIntermediate/DubinsIntermediatePlanner.h \
    gui/FlightTaskEditors/SubWidgets/TaskNameEditor.h \
    gui/FlightTaskEditors/FlightTaskEditor.h \
    gui/FlightTaskEditors/SubWidgets/CoverageTaskEditorWidgets.h \
    gui/FlightTaskEditors/SubWidgets/SamplingTaskEditorWidgets.h \
    gui/FlightTaskEditors/SamplingTaskEditor.h \
    gui/FlightTaskEditors/SubWidgets/DependencyConstraintEditor.h \
    gui/FlightTaskEditors/SubWidgets/DependencyRow.h \
    gui/FlightTaskEditors/FlyThroughTaskEditor.h \
    Serializable.h \
    Importers/Importer.h \
    Importers/GPXImporter.h

FORMS    += gui/MainWindow.ui \
    gui/PaletteWidget.ui \
    gui/PlanningControlWidget.ui \
    gui/FlightTaskAreaEditor/FlightTaskAreaObjectEditWidget.ui \
    gui/FlightTaskAreaEditor/FlightTaskRowEditor.ui \
    gui/UAVParametersWidget.ui \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintSliders.ui \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintEditor.ui \
    gui/FlightTaskEditors/SubWidgets/TaskNameEditor.ui \
    gui/FlightTaskEditors/FlightTaskEditor.ui \
    gui/FlightTaskEditors/SubWidgets/CoverageTaskEditorWidgets.ui \
    gui/FlightTaskEditors/SubWidgets/SamplingTaskEditorWidgets.ui \
    gui/FlightTaskEditors/SubWidgets/DependencyConstraintEditor.ui \
    gui/FlightTaskEditors/SubWidgets/DependencyRow.ui

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

#Linkage for GPX library.
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GPX/release/ -lGPX
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GPX/debug/ -lGPX
else:unix: LIBS += -L$$OUT_PWD/../GPX/ -lGPX

INCLUDEPATH += $$PWD/../GPX
DEPENDPATH += $$PWD/../GPX

#Linkage for Dubins library.
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Dubins/release/ -lDubins
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Dubins/debug/ -lDubins
else:unix: LIBS += -L$$OUT_PWD/../Dubins/ -lDubins

INCLUDEPATH += $$PWD/../Dubins
DEPENDPATH += $$PWD/../Dubins
