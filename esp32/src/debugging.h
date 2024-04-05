#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <SD.h>
#include "constants.h"

WiFiUDP udp;

String ssid = "";
String password = "";

File dataFile;

void sendDebugDataThroughWifi()
{
  unsigned long long currentTime = millis();

  IPAddress laptopIP = WiFi.gatewayIP();

  String message = "["
                   + String(millis())
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

void initSDCard() {
  const int CS = 13;
  const int MOSI = 27;
  const int MISO = 26;
  const int CLK = 25;

  SPI.begin(CLK, MISO, MOSI, CS);

  SD.begin(CS);

  dataFile = SD.open("data.txt", FILE_WRITE);
}

void sendDebugDataToSDCard () {

if(!currentLoc.isValid() || distanceToTarget < POSITIONAL_UNCERTAINTY_THRESHOLD) return;

String data =  String(millis()) + "," + String(currentLoc.latitude) + "," + String(currentLoc.longitude) ;

dataFile.println(data);
}
