TEMPLATE = subdirs

SUBDIRS = MapGraphics FlightPlanner QVectorND QKDTree GPX \
    WaypointPlanner

FlightPlanner.depends += MapGraphics
FlightPlanner.depends += QVectorND
FlightPlanner.depends += QKDTree
FlightPlanner.depends += GPX

WaypointPlanner.depends += MapGraphics
WaypointPlanner.depends += GPX

QKDTree.depends += QVectorND
