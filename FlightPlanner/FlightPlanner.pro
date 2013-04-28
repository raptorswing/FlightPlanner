#-------------------------------------------------
#
# Project created by QtCreator 2012-10-18T11:35:27
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += console

TARGET = FlightPlanner
TEMPLATE = app


SOURCES += main.cpp\
    gui/MainWindow.cpp \
    gui/PaletteWidget.cpp \
    gui/PlanningControlWidget.cpp \
    ProblemViewAdapter.cpp \
    MapObjects/StartPosMapObject.cpp \
    MapObjects/FlightTaskAreaMapObject.cpp \
    gui/FlightTaskAreaEditor/FlightTaskAreaObjectEditWidget.cpp \
    gui/FlightTaskAreaEditor/FlightTaskAreaListModel.cpp \
    gui/FlightTaskAreaEditor/FlightTaskDelegate.cpp \
    gui/FlightTaskAreaEditor/FlightTaskRowEditor.cpp \
    gui/UAVParametersWidget.cpp \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintSliders.cpp \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintEditor.cpp \
    gui/FlightTaskEditors/FlightTaskEditorFactory.cpp \
    gui/FlightTaskEditors/CoverageTaskEditor.cpp \
    gui/FlightTaskEditors/SubWidgets/TaskNameEditor.cpp \
    gui/FlightTaskEditors/FlightTaskEditor.cpp \
    gui/FlightTaskEditors/SubWidgets/CoverageTaskEditorWidgets.cpp \
    gui/FlightTaskEditors/SubWidgets/SamplingTaskEditorWidgets.cpp \
    gui/FlightTaskEditors/SamplingTaskEditor.cpp \
    gui/FlightTaskEditors/SubWidgets/DependencyConstraintEditor.cpp \
    gui/FlightTaskEditors/SubWidgets/DependencyRow.cpp \
    gui/FlightTaskEditors/FlyThroughTaskEditor.cpp

HEADERS  += \
    gui/MainWindow.h \
    gui/PaletteWidget.h \
    gui/PlanningControlWidget.h \
    ProblemViewAdapter.h \
    MapObjects/StartPosMapObject.h \
    MapObjects/FlightTaskAreaMapObject.h \
    gui/FlightTaskAreaEditor/FlightTaskAreaObjectEditWidget.h \
    gui/FlightTaskAreaEditor/FlightTaskAreaListModel.h \
    gui/FlightTaskAreaEditor/FlightTaskDelegate.h \
    gui/FlightTaskAreaEditor/FlightTaskRowEditor.h \
    gui/UAVParametersWidget.h \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintSliders.h \
    gui/FlightTaskEditors/SubWidgets/TimingConstraintEditor.h \
    gui/FlightTaskEditors/FlightTaskEditorFactory.h \
    gui/FlightTaskEditors/CoverageTaskEditor.h \
    gui/FlightTaskEditors/SubWidgets/TaskNameEditor.h \
    gui/FlightTaskEditors/FlightTaskEditor.h \
    gui/FlightTaskEditors/SubWidgets/CoverageTaskEditorWidgets.h \
    gui/FlightTaskEditors/SubWidgets/SamplingTaskEditorWidgets.h \
    gui/FlightTaskEditors/SamplingTaskEditor.h \
    gui/FlightTaskEditors/SubWidgets/DependencyConstraintEditor.h \
    gui/FlightTaskEditors/SubWidgets/DependencyRow.h \
    gui/FlightTaskEditors/FlyThroughTaskEditor.h

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

#Linkage for PlanningCommon library.
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PlanningCommon/release/ -lPlanningCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PlanningCommon/debug/ -lPlanningCommon
else:unix: LIBS += -L$$OUT_PWD/../PlanningCommon/ -lPlanningCommon

INCLUDEPATH += $$PWD/../PlanningCommon
DEPENDPATH += $$PWD/../PlanningCommon
