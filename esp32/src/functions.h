#include <string.h>
#include "constants.h"
#include "globals.h"

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

  float latMidpoint = (current.latitude + target.latitude) / 2;

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

  float bearing = atan(lon / lat) * 180 / PI;

  if (lon > 0 && lat < 0) // quadrant 2
  {
    bearing = bearing + 180;
  }
  else if (lon < 0 && lat < 0) // quadrant 3
  {
    bearing = bearing + 180;
  }
  else if (lon < 0 && lat > 0) // quadrant 4
  {
    bearing = bearing + 360;
  }
  return bearing;
}

void getCurrentLoc()
{
  while (gps.available())
  {
    gps.read();
    if (gps.find("$GPGLL,"))
    {
      float newLat = 999;
      float newLong = 999;
      bool validity = false;
      String nmeaSentence = gps.readStringUntil('\n');
      for (int i = 0; i < 6; i++)
      {
        int delimiterIndex = nmeaSentence.indexOf(",");
        String token = nmeaSentence.substring(0, delimiterIndex);
        token.trim();

        switch (i)
        {
        case 0: // latitude
          newLat = parseLatLong(token);
          break;
        case 1: // N/S indicator
          if (token == "S")
          {
            newLat *= -1.0;
          }
          break;
        case 2: // longitude
          newLong = parseLatLong(token);
          break;
        case 3: // S/W indicator
          if (token == "W")
          {
            newLong *= -1.0;
          }
          break;
        case 5: // validity flag
          if (token = "A")
          {
            validity = true;
          }
          break;
        default:
          break;
        }

        nmeaSentence = nmeaSentence.substring(delimiterIndex + 1);
      }
      if (!validity)
        return; // exit early if invalid

      currentLoc = Coordinates(newLat, newLong);
    }
  }
}

void getRelativeBearing()
{
  int x, y, z;

  compass.read();

  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  float temp[3];

  temp[0] = (x - CALIBRATION_B[0]);
  temp[1] = (y - CALIBRATION_B[1]);
  temp[2] = (z - CALIBRATION_B[2]);

  x = CALIBRATION_A[0][0] * temp[0] + CALIBRATION_A[0][1] * temp[1] + CALIBRATION_A[0][2] * temp[2];
  y = CALIBRATION_A[1][0] * temp[0] + CALIBRATION_A[1][1] * temp[1] + CALIBRATION_A[1][2] * temp[2];

  heading = -atan2(x, y) * 180 / PI;

  if (heading < 0)
  {
    heading += 360;
  }

  float bearingToTarget = getBearingToTarget(currentLoc, targetLoc);

  float delta = heading - bearingToTarget;

  float relativeBearing = delta;

  if (abs(delta) > 180)
  {
    relativeBearing = abs(delta) - 360;
  }
}

void navigate()
{
  // return early and do not start motors if current and target coordinates are invalid
  if (!currentLoc.isValid() || !targetLoc.isValid() || relativeBearing > 180 || relativeBearing < -180)
    return;

  float throttle = map(relativeBearing, 10, 180, MAX_ALLOWABLE_THROTTLE, MIN_PWM);

  if (abs(relativeBearing) <= BEARING_TOLERANCE_THRESHOLD) // go straignt
  {
    portMotor.writeMicroseconds(MAX_PWM);
    starboardMotor.writeMicroseconds(MAX_PWM);
  }
  else if (relativeBearing > BEARING_TOLERANCE_THRESHOLD)
  {

    portMotor.writeMicroseconds(throttle);
    starboardMotor.writeMicroseconds(MAX_ALLOWABLE_THROTTLE);
  }
  else if (relativeBearing < (0 - BEARING_TOLERANCE_THRESHOLD))
  {
    portMotor.writeMicroseconds(MAX_ALLOWABLE_THROTTLE);
    starboardMotor.writeMicroseconds(throttle);
  }
  else
  {
    portMotor.writeMicroseconds(MIN_PWM);
    starboardMotor.writeMicroseconds(MIN_PWM);
  }
}

#endif
