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

unsigned long long startupTime;

// * ===== GPS =====
SoftwareSerial gps(GPS_RX_PIN, GPS_TX_PIN);

const int GPS_UNCERTAINTY_THRESHOLD = 10;  //! value to account for gps inaccuracy

// * ===== MAGNETOMETER =====
QMC5883LCompass compass;

const int BEARING_TOLERANCE_THRESHOLD = 5;  //! value to account for magnetometer inaccuracy

const float CALIBRATION_A[3][3]{
  { 0.032112, 0.001829, -0.000664 },
  { 0.001829, 0.029941, 0.001147 },
  { -0.000664, 0.001147, 0.036735 }
};

const float CALIBRATION_B[3]{ 1467.935548, -1226.765475, -1422.699791 };

// * ===== MOTOR AND ESC =====
Servo portMotor;
Servo starboardMotor;

const int MAX_PWM = 2000;
const int MIN_PWM = 1000;

const float THROTTLE_MULTIPLIER = 1;  //! should be between 0 and 1. Double check.

const float MAX_ALLOWABLE_THROTTLE = MIN_PWM + ((MAX_PWM - MIN_PWM) * THROTTLE_MULTIPLIER);

const float STARBOARD_MOTOR_TRIM = 1;
const float PORT_MOTOR_TRIM = 0.95;

// * ===== CLASSES =====
class Coordinates {
public:
  Coordinates(float inputLat, float inputLong) {
    longitude = inputLong;
    latitude = inputLat;
  }

  float longitude;
  float latitude;

  bool isValid() {
    return (abs(latitude) <= 180.0 || abs(longitude) <= 180.0);
  }

  String toString() {
    return "Coordinates: (Longitude: " + String(longitude, 3) + ", Latitude: " + String(latitude, 3) + ")";
  }
};

#endif