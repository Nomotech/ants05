void attitudeAngle(float angle){
  // zg  0x0000 ～ 0xffff --> -90°～ 90°
  float machineAngle = zg * (90.0 / 0xffff);
  float R = angle - machineAngle;
  if(R > 90.0)  R = 90;
  if(R < -90.0) R = -90;
  R /= 90.0;

  float L = angle + machineAngle;
  if(L > 90.0)  L = 90;
  if(L < -90.0) L = -90;
  L /= 90.0;
  rightMoter  += R;
  leftMoter   += L;
}

void distination(){

}
