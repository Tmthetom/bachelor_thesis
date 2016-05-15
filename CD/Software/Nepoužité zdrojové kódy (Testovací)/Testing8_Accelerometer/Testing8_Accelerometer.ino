#include <Wire.h>

// MPU-6050 
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t aX,aY,aZ,tp,gX,gY,gZ;  // Reading values

// Precision
int prec = 500;  // How precise senzor would be (min 500 recommended)
long aXf,aYf,aZf;  // Default fixed position

// Signalization
const int led = 3;

void setup(){
  // Signalization setup
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(9600);

  // Setup MPU-6050
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);  // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  // Inicialize default fixed position
  initPosition();
}
void loop(){
  getValues();

  movement() ? digitalWrite(led, HIGH) : digitalWrite(led, LOW);
  
  delay(100);
}

boolean movement(){
  if(aX > aXf - prec && aX < aXf + prec){  // If X in precision range
    if(aY > aYf - prec && aY < aYf + prec){  // If Y in precision range
      if(aZ > aZf - prec && aZ < aZf + prec){  // If Z in precision range
        return false;  // No movement
      }
    }
  }
  return true;  // There is movement
}

void getValues(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  aX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  aY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  aZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  tp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  gX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  gY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  gZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

void initPosition(){
  
  // Setup of method
  delay(333);  // Wait till connected
  int init = 5;  // Wait for init
  int gain = 20;  // How many values into average
  
  // Wait for board init
  for (int i = 0; i < init; i++){
    getValues();  // Read values
  }

  // Count values
  for (int i = 0; i < gain; i++){
    getValues();  // Read values
    aXf += aX;  // Sum
    aYf += aY;  // Sum
    aZf += aZ;  // Sum
  }

  // Get average values
  aXf = aXf / gain;
  aYf = aYf / gain;
  aZf = aZf / gain;
}

