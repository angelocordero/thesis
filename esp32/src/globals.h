#include "constants.h"

#ifndef GLOBALS_h
#define GLOBALS_h

Coordinates currentLoc = Coordinates(999, 999);
Coordinates targetLoc = Coordinates(10.7296723, 122.5590030);

float heading = 0;
float bearingToTarget = 999;
float relativeBearing = 0;
float distanceToTarget = 0;

#endif