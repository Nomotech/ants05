void Move(float L,float R){
    // クランプ
    if(L > 1.0)  L = 1.0;
    if(L < -1.0) L =-1.0;
    if(R > 1.0)  R = 1.0;
    if(R < -1.0) R =-1.0;

    float a = 0.05;     // 安定角度閾値
    float c = 0.4;     // 最下値
    float b = 1.0 - c;  // 出力値
    // b + c = 1.0
     
    if(L > a){
        ledcWrite(moter[0],(int)((L * b + c) * 0xffff));
        ledcWrite(moter[1],0);
        ledcWrite(led[0],(int)((L * b + c) * 0xffff));
        ledcWrite(led[1],0);
    }else if(L >= -a && L <= a){
        ledcWrite(moter[0],0xffff);
        ledcWrite(moter[1],0xffff);
        ledcWrite(led[0],0);
        ledcWrite(led[1],0);
    }else if(L < -a){
        ledcWrite(moter[0],0);
        ledcWrite(moter[1],(int)((-L * b + c) * 0xffff));
        ledcWrite(led[0],0);
        ledcWrite(led[1],(int)((-L * b + c) * 0xffff));
    }

    if(R > a){
        ledcWrite(moter[2],(int)((R * b + c) * 0xffff));
        ledcWrite(moter[3],0);
        ledcWrite(led[3],(int)((R * b + c) * 0xffff));
        ledcWrite(led[2],0);
    }else if(R >= -a && R <= a){
        ledcWrite(moter[2],0xffff);
        ledcWrite(moter[3],0xffff);
    }else if(R < a){
        ledcWrite(moter[2],0);
        ledcWrite(moter[3],(int)((-R * b + c) * 0xffff));
        ledcWrite(led[3],0);
        ledcWrite(led[2],(int)((-R * b + c) * 0xffff));
    }
     // Serial.print("leftMoter:");  Serial.print(leftMoter); 
     // Serial.print(" rightMoter:"); Serial.print(rightMoter);  
     // Serial.print("\n");
}

