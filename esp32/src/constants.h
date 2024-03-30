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
SoftwareSerial gps;

const int GPS_UNCERTAINTY_THRESHOLD = 10; //! value to account for gps inaccuracy 

// * ===== MAGNETOMETER =====
QMC5883LCompass compass;

const int BEARING_TOLERANCE_THRESHOLD = 5; //! value to account for magnetometer inaccuracy

const float CALIBRATION_A[3][3]{
    {0.039132, 0.002265, -0.000295},
    {0.002265, 0.036107, 0.002331},
    {-0.000295, 0.002331, 0.044212}};

const float CALIBRATION_B[3]
    {1073.801150, -1496.911385, -1601.752288};

// * ===== MOTOR AND ESC =====
Servo portMotor;
Servo starboardMotor;

const int MAX_PWM = 2000;
const int MIN_PWM = 1000;

const float THROTTLE_MULTIPLIER = 0.5; //! should be between 0 and 1. Please double check.

const float MAX_ALLOWABLE_THROTTLE = MIN_PWM + ((MAX_PWM - MIN_PWM) * THROTTLE_MULTIPLIER);


// * ===== CLASSES =====
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

#endif

