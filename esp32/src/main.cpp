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

// hardcoded inputs
float targetLong = 0;
float targetLat = 0;

struct GPSData
{
  float longitude = 999;
  float latitude = 999;
  bool isValid = false;
};

GPSData gpsData;

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

  while (gps.available())
  {
    gps.read();
    if (gps.find("$GPGLL,"))
    {
      GPSData newGPSData = GPSData();
      String nmeaSentence;
      nmeaSentence = gps.readStringUntil('\n');
      for (int i = 0; i < 6; i++)
      {
        int delimiterIndex = nmeaSentence.indexOf(",");
        String token = nmeaSentence.substring(0, delimiterIndex);
        token.trim();

        switch (i)
        {
        case 0: // latitude
          newGPSData.latitude = parseLatLong(token);
          break;
        case 1: // N/S indicator
          if (token == "S")
          {
            newGPSData.latitude *= -1.0;
          }
          break;
        case 2: // longitude
          newGPSData.longitude = parseLatLong(token);
          break;
        case 3: // S/W indicator
          if (token == "W")
          {
            newGPSData.longitude *= -1.0;
          }
          break;
        case 5: // validity flag
          if (token = "A")
          {
            newGPSData.isValid = true;
          }
          break;
        default:
          break;
        }

        nmeaSentence = nmeaSentence.substring(delimiterIndex + 1);
      }
      if (!newGPSData.isValid)
        return; // exit early if invalid

      if (abs(newGPSData.latitude) > 180.0 || abs(newGPSData.longitude) > 180.0)
        return;

      gpsData = newGPSData;
    }


  }
}

void getMagnetometerData()
{
  int x, y;

  compass.read();

  x = compass.getX();
  y = compass.getY();

  float heading = atan2(y, x) + 180 / PI;

  float declinationAngle = (0.015 * gpsData.longitude) + 1;

  heading += declinationAngle;
}

void navigationLoop(void *parameters)
{
  getGPSData();
  getMagnetometerData();
}

void setup()
{
  Serial.begin(9600);
  Wire.begin(MAGNETOMETER_SDA_PIN, MAGNETOMETER_SCL_PIN);
}

void loop()
{
}
