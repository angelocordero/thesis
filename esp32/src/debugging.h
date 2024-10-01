#include <SPI.h>
#include <SD.h>
#include "constants.h"

void sendDataThroughSerialPort() {
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

  Serial.println(message);
}

void initSDCard() {
  const int CS = 13;
  const int MOSI = 27;
  const int MISO = 26;
  const int SCK = 25;

  SPI.begin(SCK, MISO, MOSI, CS);

  SD.begin(CS);
}

void sendDebugDataToSDCard () {
  if(!currentLoc.isValid() || distanceToTarget < POSITIONAL_UNCERTAINTY_THRESHOLD) return;

  File dataFile = SD.open("/data.csv", FILE_APPEND);

  String data =  String(millis()) + "," + String(currentLoc.latitude, 10U) + "," + String(currentLoc.longitude, 10U) + ",";

  dataFile.println(data);

  dataFile.close(); 
}

void connectToWifi() 
{
  String ssid = "drone";
  String password = "droneboat123";

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  WiFi.begin(ssid, password);
}

void sendDebugDataThroughWifi() {
  WiFiUDP udp;

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
}