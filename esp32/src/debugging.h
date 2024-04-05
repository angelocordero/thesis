#include <SPI.h>
#include <SD.h>
#include "constants.h"

File dataFile;

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
  const int CLK = 25;

  SPI.begin(CLK, MISO, MOSI, CS);

  SD.begin(CS);

  dataFile = SD.open("data.csv", FILE_WRITE);
}

void sendDebugDataToSDCard () {

if(!currentLoc.isValid() || distanceToTarget < POSITIONAL_UNCERTAINTY_THRESHOLD) return;

String data =  String(millis()) + "," + String(currentLoc.latitude) + "," + String(currentLoc.longitude) + ",";

dataFile.println(data);
}
