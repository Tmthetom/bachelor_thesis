#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX GPS Modulu
String objekt_string = "";  // Prazdny textovy retezec
char znak = ' ';  // Prazdny znak
byte gsmDriverPin[3] = {3,4,5};  // Nastaveni pinu GPS

void setup(){
  // Nasaveni GPS pinu jako vystupnich
  for(int i = 0 ; i < 3; i++){
    pinMode(gsmDriverPin[i],OUTPUT);
  }
  
  // Zahajeni komunikace
  Serial.begin(9600);  
  gps.begin(9600);

  // Nastaveni GPS
  digitalWrite(gsmDriverPin[2],HIGH);  // Reset casovace GSM
  delay(500);
  digitalWrite(gsmDriverPin[2],LOW);  // Zapnuti casovace GSM
  digitalWrite(gsmDriverPin[0],LOW);  // Zapnuti GSM
  digitalWrite(gsmDriverPin[1],HIGH);  // Vypnuti GPS
  delay(2000); 

  // Vycisteni obrazovky serial monitoru
  Serial.println("-------- Nove komunikacni okno --------");
  Serial.println();

  // Smazani vsech SMS
  gps.println("AT+CMGD=1,4"); 
}
 
void loop(){
  if (gps.available()) {
    znak = gps.read();
    objekt_string += znak;
    Serial.write(znak);
    if (znak == 'x'){
      Serial.println(objekt_string);
    }
  }
  if (Serial.available()) {
    gps.write(Serial.read());
  }
}
/*
void serialEvent(){
  int serial = gps.available();
  Serial.print("Pocet prichozich znaku: ");
  Serial.println(serial);
  if (serial > 0){
    int i;
    for (i = 0; i < serial; i++){
       Serial.print(gps.read());
    }
    Serial.println();
  }
}*/
