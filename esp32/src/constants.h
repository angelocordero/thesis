#include <ESP32Servo.h>
#include <QMC5883LCompass.h>
#include <SoftwareSerial.h>

#ifndef CONSTANTS_h
#define CONSTANTS_h

// * ===== PINOUTS =====
const int MAGNETOMETER_SDA_PIN = 21;
const int MAGNETOMETER_SCL_PIN = 22;

const int GPS_RX_PIN = 18;
const int GPS_TX_PIN = 19;

const int PORT_MOTOR_PIN = 17;
const int STARBOARD_MOTOR_PIN = 5;

// * ===== GPS =====
SoftwareSerial gps(GPS_RX_PIN, GPS_TX_PIN);

const int POSITIONAL_UNCERTAINTY_THRESHOLD = 10; //! value to account for gps inaccuracy

// * ===== MAGNETOMETER =====
QMC5883LCompass compass;

const int BEARING_TOLERANCE_THRESHOLD = 5; //! value to account for magnetometer inaccuracy

const int CRITICAL_TURNING_THRESHOLD = 150; //! if relative bearing is > critical turning threshold, turn off one motor to maximize turnign speed

const int MAGNETIC_DECLINATION = -2.06;

// calibration data for magnetometer
// see calibration process
const float CALIBRATION_A[3][3]{
  { 0.041304, -0.004433, 0.000978 },
  { -0.004433, 0.038935, 0.001932 },
  { 0.000978, 0.001932, 0.050078}
};

const float CALIBRATION_B[3]{ -1483.237651, -737.987926, -2112.775318 };

// * ===== MOTOR AND ESC =====
Servo portMotor;
Servo starboardMotor;

const int MAX_PWM = 2000;
const int MIN_PWM = 1000;

const float THROTTLE_MULTIPLIER = 1; //! should be between 0 and 1. Please double check.

const float MAX_ALLOWABLE_THROTTLE = MIN_PWM + ((MAX_PWM - MIN_PWM) * THROTTLE_MULTIPLIER);

const float STARBOARD_MOTOR_TRIM = 1;
const float PORT_MOTOR_TRIM = 0.95;

// * ===== CLASSES =====
class Coordinates
{
public:
  Coordinates(double inputLat, double inputLong)
  {
    longitude = inputLong;
    latitude = inputLat;
  }

  double longitude;
  double latitude;

  bool isValid()
  {
    return (abs(latitude) <= 180.0 || abs(longitude) <= 180.0);
  }

  String toString()
  {
    return "Coordinates: (Longitude: " + String(longitude, 10U) + ", Latitude: " + String(latitude, 10U) + ")";
  }
};

#endif
