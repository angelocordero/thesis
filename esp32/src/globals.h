#include "constants.h"

#ifndef GLOBALS_h
#define GLOBALS_h

Coordinates currentLoc = Coordinates(999, 999);
Coordinates targetLoc = Coordinates(10.7561422, 122.5448899);

float heading = 0;
float bearingToTarget = 999;
float relativeBearing = 0;
double distanceToTarget = 0;

#endif