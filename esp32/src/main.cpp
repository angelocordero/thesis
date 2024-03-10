#include <Arduino.h>
#include <QMC5883LCompass.h>
#include <SoftwareSerial.h>
#include <cstdlib>
#include <string.h>

#define MAGNETOMETER_SDA_PIN 21
#define MAGNETOMETER_SCL_PIN 22

#define GPS_RX_PIN 17
#define GPS_TX_PIN 5

#define ESC_LEFT_PIN 18
#define ESC_RIGHT_PIN 19

QMC5883LCompass compass;
SoftwareSerial gps;

class Coordinates
{
public:
  Coordinates(float inputLat, float inputLong)
  {
    longitude = inputLong;
    latitude = inputLat;
  }

  float longitude = 999;
  float latitude = 999;

  bool isValid()
  {
    return (abs(latitude) <= 180.0 || abs(longitude) <= 180.0);
  }
};

Coordinates coordinates;
Coordinates target;
float heading = 999;

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

void getGPSData()
{
  // todo test if working
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
        return; // exit early if invalidz

      coordinates = Coordinates(newLat, newLong);
    }
  }
}

void getMagnetometerData()
{
  int x, y;

  compass.read();

  x = compass.getX();
  y = compass.getY();

  heading = atan2(y, x) + 180 / PI;

  if (!coordinates.isValid())
    return;

  float declinationAngle = (0.015 * coordinates.longitude) + 1;

  heading += declinationAngle;
}

void getBearingToTarget()
{
  float x = coordinates.longitude - target.longitude;
  float y = coordinates.latitude - target.latitude;

  // todo calculate liwat
  // float theta
}

float getTargetDistance()
{
  return sqrt(sq(coordinates.latitude - target.latitude) + sq(coordinates.longitude - target.longitude));
}

void navigate()
{
  // return early and do not start motors if current and target coordinates are invalid
  if (!coordinates.isValid() || !target.isValid())
    return;

  // todo
}

void setup()
{
  Serial.begin(9600);
  Wire.begin(MAGNETOMETER_SDA_PIN, MAGNETOMETER_SCL_PIN);

  // todo assign tasks
}

void loop()
{
}
