char mpuData[14];

float mpuOffset[6];

void mpuInit(){
    SPI.begin(CLK,MISO,MOSI,SS);
    //SPI.begin(13,27,22,23);
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);

    // SSをHightに
    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);

    // ADXL345初期化
    writeRegister(107,0b10000000); delay(200);
    writeRegister(104,0b00000111); delay(200);
    writeRegister(106,0b00010000); delay(200);
    writeRegister( 27,0b00011000); delay(200);

    carib(2000);
}

void updateMpuData(){
    mpuGet();
    //加速度
    ax  = ((int16_t) mpuData[0] <<8)|(int16_t) mpuData[1];
    ay  = ((int16_t) mpuData[2] <<8)|(int16_t) mpuData[3];
    az  = ((int16_t) mpuData[4] <<8)|(int16_t) mpuData[5];

    //温度
    temp  = ((int16_t) mpuData[6] <<8)|(int16_t) mpuData[7];

    //ジャイロ
    xg_d  = ((int16_t) mpuData[8] <<8)|(int16_t) mpuData[9];
    yg_d  = ((int16_t) mpuData[10]<<8)|(int16_t) mpuData[11];
    zg_d  = ((int16_t) mpuData[12]<<8)|(int16_t) mpuData[13];

    //加速度加算
    vx += (ax - mpuOffset[0])/100;
    vy += (ay - mpuOffset[1])/100;
    vz += (az - mpuOffset[2])/100;

    //  vx = ax;
    //  vy = ay;
    //  vz = az;

    // 1/1640 = 0.00060975609 ... 2000分割のとき
    xg += (xg_d - mpuOffset[3]) * 0.00060975609;
    yg += (yg_d - mpuOffset[4]) * 0.00060975609;
    zg += (zg_d - mpuOffset[5]) * 0.00060975609;
}

void writeRegister(char registerAddress, char value) {
    digitalWrite(SS, LOW);              // SPI開始時にSSをLOWにする
    SPI.transfer(registerAddress);      // レジスタアドレス送信
    SPI.transfer(value);                // レジスタに設定する値送信
    digitalWrite(SS, HIGH);             // SPI終了時にCSをHIGHにする
}

char readRegister(char registerAddress) {
    char value;
    char address = 0x80 | registerAddress;      // 書き込みフラグを立てる
    digitalWrite(SS, LOW);                      // SPI開始時にSSをLOWにする
    SPI.transfer(address);                      // 読み出し先レジスタのアドレスを送信
    value = SPI.transfer(0x00);
    digitalWrite(SS, HIGH);                     // SPI終了時にCSをHIGHにする
    return value;
}

void readRegister2(char registerAddress,int num,char *value) {
    char address = 0x80 | registerAddress;      // 書き込みフラグを立てる
    digitalWrite(SS, LOW);                      // SPI開始時にSSをLOWにする
    SPI.transfer(address);                      // 読み出し先レジスタのアドレスを送信
    for(int i = 0;i<num ;i++){
        value[i] = SPI.transfer(0x00);
    }
    digitalWrite(SS, HIGH);                     // SPI終了時にCSをHIGHにする
}

void readRegister3(char registerAddress, int16_t numBytes, char * values) {
    char address = 0x80 | registerAddress;      // 書き込みフラグを立てる
    if(numBytes > 1)address = address | 0x40;   // 複数バイトフラグを立てる
    digitalWrite(SS, LOW);                      // SPI開始時にSSをLOWにする
    SPI.transfer(address);                      // 読み出し先レジスタのアドレスを送信
    for(int16_t i=0; i<numBytes; i++) {         // 値の読み出し
        values[i] = SPI.transfer(0x00);
    }
    digitalWrite(SS, HIGH);                     // SPI終了時にCSをHIGHにする
}

void carib(int num){
    for(int i = 0;i<num;i++){
        mpuGet();
        //加速度
        ax  = ((int16_t) mpuData[0] <<8)|(int16_t) mpuData[1];
        ay  = ((int16_t) mpuData[2] <<8)|(int16_t) mpuData[3];
        az  = ((int16_t) mpuData[4] <<8)|(int16_t) mpuData[5];

        //温度
        temp  = ((int16_t) mpuData[6] <<8)|(int16_t) mpuData[7];

        //ジャイロ
        xg_d  = ((int16_t) mpuData[8] <<8)|(int16_t) mpuData[9];
        yg_d  = ((int16_t) mpuData[10]<<8)|(int16_t) mpuData[11];
        zg_d  = ((int16_t) mpuData[12]<<8)|(int16_t) mpuData[13];

        //加速度加算
        vx += ax;
        vy += ay;
        vz += az;

        xg += xg_d;
        yg += yg_d;
        zg += zg_d;
    }
    mpuOffset[0] = vx/num;
    mpuOffset[1] = vy/num;
    mpuOffset[2] = vz/num;
    mpuOffset[3] = xg/num;
    mpuOffset[4] = yg/num;
    mpuOffset[5] = zg/num;
    vx = 0.0; vy = 0.0; vz = 0.0;
    xg = 0.0; yg = 0.0; zg = 0.0;
}

void mpuGet(){
    mpuData[0]  = readRegister(59); delayMicroseconds(100);
    mpuData[1]  = readRegister(60); delayMicroseconds(100);
    mpuData[2]  = readRegister(61); delayMicroseconds(100);
    mpuData[3]  = readRegister(62); delayMicroseconds(100);
    mpuData[4]  = readRegister(63); delayMicroseconds(100); 
    mpuData[5]  = readRegister(64); delayMicroseconds(100);
    mpuData[6]  = readRegister(65); delayMicroseconds(100);
    mpuData[7]  = readRegister(66); delayMicroseconds(100);
    mpuData[8]  = readRegister(67); delayMicroseconds(100);
    mpuData[9]  = readRegister(68); delayMicroseconds(100);
    mpuData[10] = readRegister(69); delayMicroseconds(100);
    mpuData[11] = readRegister(70); delayMicroseconds(100);
    mpuData[12] = readRegister(71); delayMicroseconds(100);
    mpuData[13] = readRegister(72); delayMicroseconds(100);
}

void mpuGet2(){
    readRegister2(59,14,mpuData);
    delay(1);
}

void mpuLog(){
    // ログ出力 
    // Serial.print("vx: ");     Serial.print(vx);   Serial.print("\t");
    // Serial.print("vy: ");     Serial.print(vy);   Serial.print("\t");
    // Serial.print("vz: ");     Serial.print(vz);   Serial.print("\t");
    // Serial.print("temp: ");   Serial.print(temp); Serial.print("\t");
    Serial.print("xg: ");     Serial.print(xg);   Serial.print("\t");
    Serial.print("yg: ");     Serial.print(yg);   Serial.print("\t");
    Serial.print("zg: ");     Serial.print(zg);   Serial.print("\n");
}
