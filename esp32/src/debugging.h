#include <WiFi.h>
#include <WiFiUdp.h>
#include "constants.h"

WiFiUDP udp;

String ssid = "";
String password = "";

void sendDebugData()
{
  unsigned long long currentTime = millis();
  double millisSinceStartup = (double)(currentTime - startupTime);

  IPAddress laptopIP = WiFi.gatewayIP();

  String message = "["
                   + String(millisSinceStartup)
                   + ", Current Loc: "
                   + currentLoc.toString()
                   + ", Target Loc "
                   + targetLoc.toString()
                   + ", Heading: "
                   + String(heading)
                   + ", Bearing to target: "
                   + String(bearingToTarget)
                   + ", Relative Bearing: "
                   + String(relativeBearing)
                   + ", Distance to target: "
                   + String(distanceToTarget)
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
