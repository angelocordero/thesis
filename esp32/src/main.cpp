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
     vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void gpsTask(void *parameters)
{
  for (;;)
  {
    getCurrentLoc();
     vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void debugTask(void *parameters){
  for(;;){
    sendDebugDataToSDCard();
     vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  // Serial.begin(115200); //! remove on final deployment to save(?) resources
  compass.init();
  gps.begin(9600);
  gps.enableRx(true);
  gps.enableTx(false);

  portMotor.attach(PORT_MOTOR_PIN, MIN_PWM, MAX_PWM);
  starboardMotor.attach(STARBOARD_MOTOR_PIN, MIN_PWM, MAX_PWM);
  portMotor.writeMicroseconds(MIN_PWM);
  starboardMotor.writeMicroseconds(MIN_PWM);

  initSDCard();

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

  delay(3000);

  xTaskCreatePinnedToCore(
      motorsTask,
      "Motors Task",
      3000,
      NULL,
      1,
      NULL,
      secondaryCore //
  );

    xTaskCreatePinnedToCore(
      debugTask,
      "Debug task",
      3000,
      NULL,
      1,
      NULL,
      mainCore //
  );

}

void loop() {
  getRelativeBearing();
  delay(100);
}