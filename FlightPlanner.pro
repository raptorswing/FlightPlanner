TEMPLATE = subdirs

SUBDIRS = MapGraphics FlightPlanner QVectorND QKDTree GPX WaypointPlanner Dubins PlanningCommon

FlightPlanner.depends += MapGraphics
FlightPlanner.depends += QVectorND
FlightPlanner.depends += QKDTree
FlightPlanner.depends += GPX
FlightPlanner.depends += PlanningCommon

WaypointPlanner.depends += MapGraphics
WaypointPlanner.depends += GPX
WaypointPlanner.depends += Dubins
WaypointPlanner.depends += PlanningCommon
WaypointPlanner.depends += QVectorND
WaypointPlanner.depends += QKDTree

PlanningCommon.depends += MapGraphics
PlanningCommon.depends += QVectorND
PlanningCommon.depends += QKDTree
PlanningCommon.depends += Dubins

QKDTree.depends += QVectorND
