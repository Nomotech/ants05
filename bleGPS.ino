/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <sstream>
void getDistance(int rssi, int txPower);


int scanTime = 5; //In seconds
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    int rssi = advertisedDevice.getRSSI();
    int txPower = advertisedDevice.getTXPower();
    const char* deviceName = advertisedDevice.getName().c_str();
    int moduleX, moduleY;
    if(sscanf(deviceName,"M:%d:%d",&moduleX,&moduleY) == 2){
    	Serial.printf("Advertised Device: %d %d \n", moduleY,moduleY);
      Serial.printf("Advertised RSSI: %d \n", rssi);
      Serial.printf("Advertised TxPower: %d \n", txPower);
      getDistance(rssi , txPower);
    }
  }
};

void getDistance(int rssi, int txPower){
  double d = pow(10.0 , ((double)(txPower - rssi) / 20.0));
  Serial.printf("Advertised Distance: %f \n",d);
}

void bleScanSetup() {
  BLEDevice::init("");
}

int scanAdv(){
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  BLEScanResults foundDevices = pBLEScan->start(scanTime);
  return 0;
}

void bleGPS(){
	scanAdv();
}
 