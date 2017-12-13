void attitudeAngle(float angle){
  // zg  0x0000 ～ 0xffff --> -90°～ 90°

  //float T = machineT;
  float T = mpuMachineT;
  float R = angle * PI/180 - T;
  if(R > PI/2)  R = PI/2;
  if(R < -PI/2) R = -PI/2;
  R /= PI/2;

  float L = angle * PI/180 + T;
  if(L >PI/2)  L = PI/2;
  if(L < -PI/2) L = -PI/2;
  L /= PI/2;
  rightMoter  += R;
  leftMoter   += L;
}

void distination(){

}
