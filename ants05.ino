#include <SPI.h>
#include <math.h>
#include "freertos/task.h"

// -----------------------------< pinset >--------------------------------
const int moterPin[]    = {16,17,18,19};
const int moter[]    = {0,1,2,3};
const int button[] = {0};
const int ledPin[] = {12,2,5,21};
const int led[] = {4,5,6,7};
//0b 35 34 32 33 26 27 14 12

#define ENC_R 4
#define ENC_L 15
#define CLK 22
#define MISO 27
#define MOSI 13
#define SS 23
#define BW_RATE     0x2C    //Data rate and power mode control
#define POWER_CTL   0x2D    //Power Control Register
#define DATA_FORMAT 0x31    //Data format control

#define PI 3.1415926535897932384626433832795

// --------------------------------< data >----------------------------------------------
float tireDistance = 27.0;      // 機体中心軸からタイヤまでの距離
float tireRadius = 12.3;        // タイヤ半径
// タイヤスリット 30 --> 60ppr
// 各タイヤ走行距離 = (タイヤ半径) * (回転角) = (タイヤ半径) * (encoderCount / 60) * PI

int16_t xa_d=0,ya_d=0,za_d=0;
int16_t xg_d=0,yg_d=0,zg_d=0;
float xa = 0.0, ya = 0.0, za = 0.0;
float xg = 0.0, yg = 0.0, zg = 0.0;
float temp = 0.0;

float rightMoter=0.0;
float leftMoter=0.0;
    
bool encoderR = false;
bool encoderL = false;
bool oldEncoderR = false;
bool oldEncoderL = false;
int encoderCountR = 0;
int encoderCountL = 0;
int encoderCountR_ = 0;
int encoderCountL_ = 0;
int Threshold = 1000;

float machineX = 0.0; //機体座標X(mm)
float machineY = 0.0; //機体座標Y(mm)
float machineT = 0.0; //機体角度Theta(rad)   
float encoderT = 0.0; //エンコーダTheta(rad)
float gyroT = 0.0;    //ジャイロTheta(SI)

bool startFlag = false; 
bool sw0 = false;
bool oldsw0 = false; 

// -----------------------------<>-----------------------------------
void update(){
  rightMoter = 0.0;
  leftMoter = 0.0;
}


void MainTask(void* arg) {
  while (1) {
    attitudeAngle(0.0);               // 機体の姿勢角
    Move(leftMoter,rightMoter);       // モーターに出力値を与える
    update();                         // update
    delay(10);
  }
}

// ルーチンをこなす
void SensorTask(void* arg) {
  while (1) {
    updateMpuData();          // mpu6500からのデータ更新
    updateEncoder();          // エンコーダの値を更新
    updateMachinePosition();  // 機体座標の更新
    delay(10);
  }
}

void setup() {
  // ------------------------< init >---------------------------
  Serial.print("init\n");
  for (int i = 0; i < 4; i++){
    pinMode(moterPin[i],  OUTPUT);
    ledcAttachPin(moterPin[i], moter[i]);
    ledcSetup(moter[i],1000000,16);
  }
  for (int i = 0; i < 4; i++){
    pinMode(ledPin[i], OUTPUT);
    ledcAttachPin(ledPin[i], led[i]);
    ledcSetup(led[i], 1000000, 16);     //DRV8835のpwm最大周波数は250kHzまで
  }
  for (int i = 0; i < 1; i++) pinMode(button[i], INPUT);
  mpuInit();
  int i = 0;
  for(i=0;i<=65000;i+=100){
    ledcWrite(led[0],i);
    delay(1);
  }
  
  // ---------------------< RTOSのタスク振り >----------------------------
  xTaskCreatePinnedToCore(MainTask,   "MainTask",   4096, NULL, 1, NULL, 0);  // core0
  xTaskCreatePinnedToCore(SensorTask, "SensorTask", 4096, NULL, 1, NULL, 1);  // core1
}

void loop() {}
