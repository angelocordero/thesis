#include <cstdlib>
#include <string.h>

#include "constants.h"
#include "functions.h"

Coordinates currentLoc;
Coordinates targetLoc;
float bearing = 999;

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

  float heading = -atan2(x, y) * 180 / PI;

  if (heading < 0)
  {
    heading += 360;
  }

  float bearingToTarget = getBearingToTarget(currentLoc, targetLoc);

  float delta = heading - bearingToTarget;

  float bearing = delta;

  if (abs(delta) > 180)
  {
    bearing = abs(delta) - 360;
  }
}

void navigate()
{
  // return early and do not start motors if current and target coordinates are invalid
  if (!currentLoc.isValid() || !targetLoc.isValid() || bearing > 180 || bearing < -180)
    return;

  float throttle = map(bearing, 10, 180, MAX_ALLOWABLE_THROTTLE, MIN_PWM);

  if (abs(bearing) <= BEARING_TOLERANCE_THRESHOLD) // go straignt
  {
    portMotor.writeMicroseconds(MAX_PWM);
    starboardMotor.writeMicroseconds(MAX_PWM);
  }
  else if (bearing > BEARING_TOLERANCE_THRESHOLD)
  {

    portMotor.writeMicroseconds(throttle);
    starboardMotor.writeMicroseconds(MAX_ALLOWABLE_THROTTLE);
  }
  else if (bearing < (0 - BEARING_TOLERANCE_THRESHOLD))
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

void setup()
{
  Serial.begin(9600); //! remove on final deployment to save(?) resources
  compass.init();

  portMotor.attach(PORT_MOTOR_PIN, MIN_PWM, MAX_PWM);
  starboardMotor.attach(STARBOARD_MOTOR_PIN, MIN_PWM, MAX_PWM);
  portMotor.writeMicroseconds(MIN_PWM);
  starboardMotor.writeMicroseconds(MIN_PWM);
  delay(3000);

  // todo assign tasks
}

void loop()
{
}
