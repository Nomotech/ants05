void attitudeAngle(float angle){
  // zg  0x0000 ～ 0xffff --> -90°～ 90°
  float T = machineT;

  float R = angle * PI/180.0 - T;
  if(R > PI)  R = PI;
  if(R < -PI) R = -PI;
  R /= PI;

  float L = angle * PI/180.0 + T;
  if(L >PI)  L = PI;
  if(L < -PI) L = -PI;
  L /= PI;
  rightMoter  += R;
  leftMoter   += L;
  Serial.printf("L:%.2f,R:%.2f\n",leftMoter,rightMoter);
}

void distination(){

}
