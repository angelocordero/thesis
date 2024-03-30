#include <string.h>
#include "constants.h"

#ifndef FUNCTIONS_h
#define FUNCTIONS_h

float parseLatLong(String input)
{
  int decimalIndex = input.indexOf('.');
  if (decimalIndex == -1)
  {
    return input.toFloat();
  }

  String degreeString = input.substring(0, decimalIndex - 2);
  String minuteString = input.substring(decimalIndex - 2, input.length() - degreeString.length());

  return degreeString.toFloat() + (minuteString.toFloat() / 60.0);
}

float getTargetDistance(Coordinates current, Coordinates target) // pythagorean theorem
{
  // todo: TESTING

  float lon = current.longitude - target.longitude;
  float lat = current.latitude - target.latitude;

  float latMidpoint = ( current.latitude + target.latitude  ) / 2;

  float lonMetersPerDegree = 111412.84 * cos(latMidpoint) - 93.5 * cos(3 * latMidpoint) + 0.118 * cos(5 * latMidpoint);
  float latMetersPerDegree = 111132.92 - 559.82 * cos(2 * latMidpoint) + 1.175 * cos(4 * latMidpoint) - 0.0023 * cos(6 * latMidpoint);

  return sqrt(sq(lon * lonMetersPerDegree) + sq(lat * latMetersPerDegree));
}

float getBearingToTarget(Coordinates current, Coordinates target)
{
  // todo: TESTING
  // todo: test using atan2

  float lon = current.longitude - target.longitude;
  float lat = current.latitude - target.latitude;

  float theta = atan(lon / lat) * 180 / PI;

  if (lon > 0 && lat < 0) // quadrant 2
  {
    theta = theta + 180;
  }
  else if (lon < 0 && lat < 0) // quadrant 3
  {
    theta = theta + 180;
  }
  else if (lon < 0 && lat > 0) // quadrant 4
  {
    theta = theta + 360;
  }
  return theta;
}

#endif
