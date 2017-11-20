void updateEncoder(){
	if(oldEncoderR != encoderR){
		if(rightMoter > 0)	encoderCountR+=1;
		if(rightMoter < 0)	encoderCountR-=1;
	}
	if(oldEncoderL != encoderL){
		if(leftMoter > 0)	encoderCountL+=1;
		if(leftMoter < 0)	encoderCountL-=1; 
	}

  oldEncoderR = encoderR;
  oldEncoderL = encoderL;
  encoderR = (analogRead(ENC_R) > Threshold) ? true : false;
  encoderL = (analogRead(ENC_L) > Threshold) ? true : false;

//   Serial.print(" enco ang R: "); Serial.print(analogRead(ENC_R));
//   Serial.print(" enco ang L: "); Serial.print(analogRead(ENC_L));
//   Serial.printf("\n");

//   Serial.print(" enco value R: "); Serial.print(encoderR);
//   Serial.print(" enco value L: "); Serial.print(encoderL);
//   Serial.printf("\n");
  
//	 Serial.print(" enco count R: "); Serial.print(encoderCountR);
//	 Serial.print(" enco count L: "); Serial.print(encoderCountL);
//	 Serial.print("\n");
}

void updateMachinePosition(){
  float dR = (float)(encoderCountR - encoderCountR_) / 60.0 * tireRadius * PI;
  float dL = (float)(encoderCountL - encoderCountL_) / 60.0 * tireRadius * PI;
  encoderCountR_ = encoderCountR;
  encoderCountL_ = encoderCountL;
  
  float r = 0.0;
  if(dR != dL) r = (float)(dR + dL) * tireDistance / (dR - dL); //カーブの回転半径
  float dTheta = (float)(dR - dL) / (2 * tireDistance);         //角度の変化量
  float dX = r * sin(dTheta);                                   //１ループ前の機体から見たXの変化量
  float dY = r * (1.0-cos(dTheta));                             //１ループ前の機体から見たYの変化量
  encoderT += dTheta;
  machineX += dX * cos(encoderT) - dY * sin(encoderT);
  machineY += dX * sin(encoderT) + dY * cos(encoderT);
  machineT = gyroT;

  // Serial.print("encoderCountR_: ");  Serial.print(encoderCountR_); Serial.print("\t");
  // Serial.print("encoderCountL_: ");  Serial.print(encoderCountL_); Serial.print("\t");
  // Serial.print("r: "); Serial.print(r);  Serial.print("\t");
  // Serial.print("dX: ");  Serial.print(dX); Serial.print("\t");
  // Serial.print("dY: ");  Serial.print(dY); Serial.print("\t");

  Serial.print("machineX: "); Serial.print(machineX); Serial.print("\t");
  Serial.print("machineY: "); Serial.print(machineY); Serial.print("\t");
  Serial.print("encoderT: "); Serial.print(encoderT); Serial.print("\t");
  Serial.print("\n");
}

void debug(){
    //pasteraterm
    Serial.print(":R:"); 
    Serial.print((int)xa/1000); Serial.print(":"); 
    Serial.print((int)ya/1000); Serial.print(":");
    Serial.print((int)za/1000); Serial.print(":");Serial.print("\n");
    Serial.print(":B:");
    Serial.print((int)xg); Serial.print(":"); 
    Serial.print((int)yg); Serial.print(":");
    Serial.print((int)zg); Serial.print(":");Serial.print("\n");
}
