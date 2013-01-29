TEMPLATE = subdirs

SUBDIRS = MapGraphics FlightPlanner QVectorND QKDTree

FlightPlanner.depends += MapGraphics
FlightPlanner.depends += QVectorND
FlightPlanner.depends += QKDTree

QKDTree.depends += QVectorND
