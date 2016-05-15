#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

// Výchozí I2C adres je 0x68
MPU6050 accelgyro;

// získané hodnoty
int16_t ax, ay, az;
int16_t gx, gy, gz;

#define LED_PIN 3
bool blinkState = false;

void setup(){
  // připojení k I2C sběrnici, I2Cdev to sama nedělá
  Wire.begin();
  
  // Inicializace UART komunikace
  Serial.begin(9600);
  
  // Inicializace gyra
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  
  // Test komunikace s gyrem
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  // Nastavíme diodu na výstupu D13
  pinMode(LED_PIN, OUTPUT);

  // Citlivost akcelerometru
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
}

void loop(){
  
  // Přečtení hodnot z gyra a akcelerometru
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Zobrazení jednotlivých hodnot (oddělené taby)
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);
}
