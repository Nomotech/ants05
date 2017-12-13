/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "WiFi.h"

void WiFiSetup() {
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");
}

void getWiFiAdv(){
  //Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  //int n = WiFi.scanNetworks();
  int n = WiFi.scanNetworks(false,false,true,3,10);
  //Serial.println("scan done");
  if (n == 0) {
    //Serial.println("no networks found");
  } else {
    Serial.print(n);
    //Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      // if (WiFi.SSID(i).indexOf("Assist") >= 0) {
          Serial.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")\n");
          //delay(10);
      // }
    }
  }
  //Serial.println("");

  // Wait a bit before scanning again
  //delay(1);
}
