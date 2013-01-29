TEMPLATE = subdirs

SUBDIRS = MapGraphics FlightPlanner QVectorND

FlightPlanner.depends += MapGraphics
FlightPlanner.depends += QVectorND