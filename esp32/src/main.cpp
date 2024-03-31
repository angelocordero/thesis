#include <cstdlib>
#include <string.h>

#include "constants.h"
#include "functions.h"
#include "debugging.h"
#include "globals.h"


void motorsTask(void *parameters)
{
  for (;;)
  {
    navigate();
  }
}

void gpsTask(void *parameters)
{
  for (;;)
  {
    getCurrentLoc();
  }
}

void magnetometerTask(void *parameters)
{
  for (;;)
  {
    getRelativeBearing();
  }
}

void debugTask(void *parameters){
  for(;;){
    sendDebugData(currentLoc, targetLoc, heading, bearingToTarget, relativeBearing);
  }
}

void setup()
{
  // Serial.begin(9600); //! remove on final deployment to save(?) resources
  startupTime = millis();
  compass.init();

  portMotor.attach(PORT_MOTOR_PIN, MIN_PWM, MAX_PWM);
  starboardMotor.attach(STARBOARD_MOTOR_PIN, MIN_PWM, MAX_PWM);
  portMotor.writeMicroseconds(MIN_PWM);
  starboardMotor.writeMicroseconds(MIN_PWM);

  connectToWifi();

  int mainCore = CONFIG_ARDUINO_RUNNING_CORE;
  int secondaryCore = mainCore == 0 ? 1 : 0;

  xTaskCreatePinnedToCore(
      gpsTask,
      "GPS Task",
      3000,
      NULL,
      1,
      NULL,
      secondaryCore //
  );

  xTaskCreatePinnedToCore(
      magnetometerTask,
      "Magnetometer task",
      3000,
      NULL,
      1,
      NULL,
      secondaryCore //
  );

  xTaskCreatePinnedToCore(
      magnetometerTask,
      "Magnetometer task",
      3000,
      NULL,
      1,
      NULL,
      mainCore //
  );

  delay(3000);

  xTaskCreatePinnedToCore(
      motorsTask,
      "Motors Task",
      3000,
      NULL,
      1,
      NULL,
      mainCore //
  );
}
