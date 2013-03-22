TEMPLATE = subdirs

SUBDIRS = MapGraphics FlightPlanner QVectorND QKDTree GPX Dubins

FlightPlanner.depends += MapGraphics
FlightPlanner.depends += QVectorND
FlightPlanner.depends += QKDTree
FlightPlanner.depends += GPX
FlightPlanner.depends += Dubins

QKDTree.depends += QVectorND
