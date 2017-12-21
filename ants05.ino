#include <SPI.h>
#include <math.h>
#include "freertos/task.h"
#include "SimpleBLE.h"

// -----------------------------< pinset >--------------------------------
//書込&ユーザースイッチ　GPIO0
//
//LED1　GPIO12
//LED2　GPIO2
//LED3　GPIO5
//LED4　GPIO21
//
//SPI(ジャイロ・加速度)
//CLK　GPIO22
//MISO　GPIO27
//MOSI　GPIO13
//CS(SS)　GPIO23
//
//I2C(ToF)
//SCK　GPIO33
//SDA　GPIO32
//GPIO1　GPIO26
//
//モタドラ
//A1　GPIO19
//A2　GPIO18
//B1　GPIO16
//B2　GPIO17
//
//アナログI or デジタルIO
//うしろ　GPIO15
//うしろ　GPIO4
//まえ　GPIO39(入力のみ)
//まえ　GPIO36(入力のみ)
//まえ　GPIO14
//まえ　GPIO25

const int moterPin[]    = {16,17,18,19};
const int button[] = {0};
const int ledPin[] = {12,2,5,21};
const int servoPin[] = {14,25};

//0b 35 34 32 33 26 27 14 12
const int moter[]    = {0,1,2,3}; // moter attach
const int led[] = {4,5,6,7};
const int servo[] = {8,9};

#define ENC_R 39
#define ENC_L 36
#define CLK 22
#define MISO 27
#define MOSI 13
#define SS 23
#define BW_RATE     0x2C    //Data rate and power mode control
#define POWER_CTL   0x2D    //Power Control Register
#define DATA_FORMAT 0x31    //Data format control

#define PI 3.1415926535897932384626433832795


struct PassingPoint {
  float x = 0.0;
  float y = 0.0;
  float ps = 0.0;
  float ls = 0.0;
  struct PassingPoint *next = NULL; 
};
// --------------------------------< global変数 >----------------------------------------------
const float tireDistance = 27.0;      // 機体中心軸からタイヤまでの距離
const float tireRadius = 12.3;        // タイヤ半径
const int threshold = 1000;           // encoder閾値
const int ppr = 60;                   // タイヤスリット 30 --> 60ppr

int16_t ax=0,ay=0,az=0;       // 加速度変化量
int16_t xg_d=0,yg_d=0,zg_d=0;       // ジャイロ変化量
float vx = 0.0, vy = 0.0, vz = 0.0; // 速度(加速度積算)
float xg = 0.0, yg = 0.0, zg = 0.0; // ジャイロ積算
float temp = 0.0;                   // 温度
    
bool encoderR = false;
bool encoderL = false;
bool oldEncoderR = false;
bool oldEncoderL = false;
int encoderCountR = 0;
int encoderCountL = 0;
int encoderCountR_ = 0;
int encoderCountL_ = 0;

float encoderT = 0.0;   //エンコーダTheta(rad)
float gyroT = 0.0;      //ジャイロTheta(SI)

float machineX = 0.0; //機体座標X(mm)
float machineY = 0.0; //機体座標Y(mm)
float machineT = 0.0; //機体角度Theta(rad) 

float mpuMachineX = 0.0; //機体座標X(mm)
float mpuMachineY = 0.0; //機体座標Y(mm)
float mpuMachineT = 0.0; //機体角度Theta(rad) 

float rightMoter=0.0; // 右moter出力 -1.0 ~ 1.0
float leftMoter=0.0;  // 左moter出力 -1.0 ~ 1.0

float debugX =0.0;
float debugY =0.0;
float debugT =0.0;

struct PassingPoint *route;
struct PassingPoint *nextPoint;

// -----------------------------<>-----------------------------------
void update(){
 rightMoter = 0.0;
 leftMoter = 0.0;
}


void MainTask(void* arg) {
  int count = 0;
  while (1) {
    updateMpuData();          // mpu6500からのデータ更新
    //mpuLog();
    updateEncoder();          // エンコーダの値を更新
    updateMachinePosition();  // 機体座標の更新
    switch(count%2){
      case 0: Serial.printf("r:%.2f:%.2f:%.2f\n",machineX,machineY,machineT); break;
      case 1: Serial.printf("b:%.2f:%.2f:%.2f\n",nextPoint->x,nextPoint->y,debugT); break;
      //case 2: Serial.printf("y:%.2f:%.2f:%.2f\n",vx,vy,vz); break;
    }
    //Serial.printf("r:%.2f:%.2f:%.2f\n",machineX,machineY,machineT);
    //attitudeAngle(0.0);               // 機体の姿勢角になるようにmoter出力を変える
    update();                         // update
    runToPoint(nextPoint);
    Move(leftMoter,rightMoter);       // モーターに出力値を与える
    count++;
    delay(10);
  }
}

// ルーチンをこなす
void SensorTask(void* arg) {
  while (1) {
    bleGPS();
    delay(10);
  }
}

void setup() {
  // ------------------------< init >---------------------------
  Serial.begin(115200);
  Serial.print("init\n");
  // pinset
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
  // for (int i = 0; i < 2; i++){
  //   pinMode(servoPin[i], OUTPUT);
  //   ledcAttachPin(servoPin[i], servo[i]);
  //   ledcSetup(servo[i], 50, 16);     //DRV8835のpwm最大周波数は250kHzまで
  // }
  
  for (int i = 0; i < 1; i++) pinMode(button[i], INPUT);
  bleScanSetup();
  mpuInit();
  int i = 0;
  for(i=0;i<=65000;i+=100){ ledcWrite(led[0],i); delay(1); }  //完了の合図
  
  // route set
  route = (PassingPoint *)malloc(sizeof(PassingPoint));
  route->x = route->y = route->ps = route->ls = 0.0; route->next =NULL;
  nextPoint = route;
  nextPoint = addPassingPoint(nextPoint,  0.0,  10.0, 0.4,0.0);
  nextPoint = addPassingPoint(nextPoint, 10.0,  10.0, 0.4,0.0);
  nextPoint = addPassingPoint(nextPoint, 10.0,  0.0,  0.4,0.0);
  nextPoint = addPassingPoint(nextPoint,  0.0,  0.0,  0.4,0.0);
  nextPoint = route;
  // printRoute(route);

  Serial.printf("init finished\n");
  // ---------------------< RTOSのタスク振り >----------------------------
  xTaskCreatePinnedToCore(MainTask,   "MainTask",   4096, NULL, 1, NULL, 0);  // core0
  xTaskCreatePinnedToCore(SensorTask, "SensorTask", 4096, NULL, 1, NULL, 1);  // core1
}

void loop() {}
