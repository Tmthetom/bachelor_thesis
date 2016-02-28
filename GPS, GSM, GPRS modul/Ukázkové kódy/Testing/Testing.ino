#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX

byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS
int ledpin = 13;  // Signalization LED
String string = "";  // Empty string
boolean lock = false;  // Car is not locked
String phone = "+420776006865"  // Default phone number

void setup(){     
  for(int i = 0 ; i < 3; i++){pinMode(gsmDriverPin[i],OUTPUT);}
  pinMode(ledpin,OUTPUT);  // Setting of LED pin
  
  Serial.begin(9600);  // Connect with PC (Serial monitor)
  gps.begin(9600);  // Connect with GPS (GPS/GPRS/GSM Module v3.0)
  
  digitalWrite(5,HIGH);  // Reset GSM timer
  delay(1500);
  digitalWrite(5,LOW);  // Enable GSM timer
  digitalWrite(3,LOW);  // Enable GSM
  digitalWrite(4,HIGH);  // Disable GPS
  delay(5000);
  delay(5000);
  delay(5000);

  sendCommand("AT+CMGD=1,4");  // Delete all SMS
  digitalWrite(ledpin,HIGH);  // GPS ready sign
}
 
void loop(){
  if(gps.available() > 0){
    string = "";  // Clear string
    while (gps.available() > 0) {
      string += gps.readString();
    }
    Serial.println(string);  // Send data to PC
    string.trim();  // Delete any starting and ending whitespaces
    if(string.equalsIgnoreCase("+CMTI: \"SM\",1")){
      Serial.println("NEW SMS");
    }
  }
}

void sendCommand(String command){
  delay(500);
  Serial.print("Arduino -> GPS: ");
  Serial.println(command);  // Send data to PC
  gps.println(command);  // Send data to GPS
  delay(500);
}

void sendSMS(String sms){
  
}
}

