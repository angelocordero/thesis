#include <string.h>
#include "constants.h"
#include "globals.h"

#ifndef FUNCTIONS_h
#define FUNCTIONS_h

double parseLatLong(String input) {
  int decimalIndex = input.indexOf('.');
  if (decimalIndex == -1) {
    return input.toDouble();
  }

  String degreeString = input.substring(0, decimalIndex - 2);
  String minuteString = input.substring(decimalIndex - 2, input.length());

  return degreeString.toDouble() + (minuteString.toDouble() / 60.0);
}

void getDistanceToTarget() 
{
  Coordinate p1 = currentLoc;
  Coordinate p2 = targetLoc;

  double latRad1 = p1.latitude * pi / 180;
  double latRad2 = p2.latitude * pi / 180;
  double latRadDiff = (p2.latitude - p1.latitude) * pi / 180;
  double lonRadDiff = (p2.longitude - p1.longitude) * pi / 180;

  double a = pow(sin(latRadDiff / 2), 2) + 
             pow(sin(lonRadDiff / 2), 2) *
             cos(latRad1) * cos(latRad2);

  distanceToTarget = 2 * 6378137 * asin(sqrt(a));
}

void getBearingToTarget()
{
  // todo: TESTING
  // todo: test using atan2

  float lon = currentLoc.longitude - targetLoc.longitude;
  float lat = currentLoc.latitude - targetLoc.latitude;

  if(lon == 0 && lat < 0) // 0 degrees
  {
    bearingToTarget = 0;
    return;
  } else if(lon < 0 && lat == 0 ) // 90 degrees
  {
    bearingToTarget = 90;
    return;
  } else if (lon == 0 && lat > 0) // 180 degrees
  {
    bearingToTarget = 180;
    return;
  }else if (lon > 0 && lat == 0) // 270 degrees
  {
    bearingToTarget = 270;
    return;
  }

  float bearing = atan(lon / lat) * 180 / PI;

  if (lon < 0 && lat < 0) // quadrant 1
  {
    bearingToTarget = bearing;
  }
  else if (lon > 0 && lat < 0) // quadrant 4
  {
    bearingToTarget = bearing + 360;
  }
  else // quadrant 2 and 3
  {
    bearingToTarget = bearing + 180;
  }
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
      for (int i = 0; i < 7; i++)
      {
        int delimiterIndex = nmeaSentence.indexOf(",");
        String token = nmeaSentence.substring(0, delimiterIndex);
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
        case 3: // E/W indicator
          if (token == "W")
          {
            newLong *= -1.0;
          }
          break;
        case 5: // validity flag
          validity = (token == "A");
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

  float tempHeading = -atan2(x, y) * 180 / PI;

  tempHeading = tempHeading + MAGNETIC_DECLINATION;

  if (tempHeading < 0)
  {
    heading = tempHeading + 360;
  }
  else
  {
    heading = tempHeading;
  }

  getBearingToTarget();

  float delta = heading - bearingToTarget;

  if (abs(delta) > 180)
  {
    relativeBearing = abs(delta) - 360;
  }
  else
  {
    relativeBearing = delta;
  }
}

void navigate()
{
  // return early and do not start motors if current and target coordinates are invalid
  if (!currentLoc.isValid() || !targetLoc.isValid() || relativeBearing > 180 || relativeBearing < -180)
    return;

  getDistanceToTarget();

  if (distanceToTarget < POSITIONAL_UNCERTAINTY_THRESHOLD)
  {
    // drone is near the target;
    portMotor.writeMicroseconds(MIN_PWM);
    starboardMotor.writeMicroseconds(MIN_PWM);
    return;
  }

  float constrainedBearing = constrain(relativeBearing, -CRITICAL_TURNING_THRESHOLD, CRITICAL_TURNING_THRESHOLD);

  float throttle = map(abs(constrainedBearing), BEARING_TOLERANCE_THRESHOLD, CRITICAL_TURNING_THRESHOLD, MAX_ALLOWABLE_THROTTLE, MIN_PWM);

  if (abs(relativeBearing) <= BEARING_TOLERANCE_THRESHOLD) // go straignt
  {
    float portThrust = MIN_PWM + ((MAX_ALLOWABLE_THROTTLE - MIN_PWM) * PORT_MOTOR_TRIM);
    float starboardThrust = MIN_PWM + ((MAX_ALLOWABLE_THROTTLE - MIN_PWM) * STARBOARD_MOTOR_TRIM);

    portMotor.writeMicroseconds(portThrust);
    starboardMotor.writeMicroseconds(starboardThrust);
  }
  else if (relativeBearing > BEARING_TOLERANCE_THRESHOLD) // turn left
  {
    portMotor.writeMicroseconds(throttle);
    starboardMotor.writeMicroseconds(MAX_ALLOWABLE_THROTTLE);
  }
  else if (relativeBearing < (0 - BEARING_TOLERANCE_THRESHOLD)) // turn right
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
