TEMPLATE = subdirs

SUBDIRS = MapGraphics FlightPlanner QVectorND QKDTree GPX WaypointPlanner Dubins

FlightPlanner.depends += MapGraphics
FlightPlanner.depends += QVectorND
FlightPlanner.depends += QKDTree
FlightPlanner.depends += GPX
FlightPlanner.depends += Dubins

WaypointPlanner.depends += MapGraphics
WaypointPlanner.depends += GPX

QKDTree.depends += QVectorND
