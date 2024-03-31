//! try lang

#include <WiFi.h>
#include <WiFiUdp.h>
#include "constants.h"

WiFiUDP udp;

String ssid = "";
String password = "";

unsigned long long startupTime;

void sendDebugData(Coordinates current, Coordinates target, float heading, float bearingToTarget, float relativeBearing)
{
  unsigned long long currentTime = millis();
  double millisSinceStartup = (double)(currentTime - startupTime);

  IPAddress laptopIP = WiFi.gatewayIP();

  String message = "[" 
  + String(millisSinceStartup) 
  + ", Current Loc: " 
  + current.toString() 
  + ", TargetLoc " 
  + target.toString() 
  + ", Heading: " 
  + String(heading, 3) 
  + ", Bearing to target: " 
  + String(bearingToTarget, 3) 
  + ", Relative Bearing: " 
  + String(relativeBearing, 3) 
  + "]";

  udp.beginPacket(laptopIP, 5000);
  udp.print(message);
  udp.endPacket();

  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void connectToWifi()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  WiFi.begin(ssid, password);
}
