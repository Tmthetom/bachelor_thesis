#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX

byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS
int ledpin = 13;  // Signalization LED
char string[] = "";  // Empty string

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
  delay(2000);
  delay(5000);
  delay(5000);

  sendCommand("AT+CMGD=1,4");  // Delete all SMS
  digitalWrite(ledpin,HIGH);  // GPS ready sign
}
 
void loop(){
  if(gps.available() > 0){
    memset(string, 0, sizeof string);  // Clear string
    int i = 0;
    while (gps.available() > 0) {
      string[i] += gps.read();
      i = i++;
    }
    Serial.println(string);  // Send data to PC
  }
}
  
  /*
  if(gps.available()>0)
  {
    inchar=gps.read();
    Serial.write(inchar);
    if(inchar=='T')
    {
      delay(10);
      inchar=gps.read(); 
      Serial.write(inchar);
      if (inchar=='I')                                      //When the GSM module get the message, it will display the sign '+CMTI "SM", 1' in the serial port
      {      
        Serial.println("GPS: Prisla SMS");
        delay(10);
        Serial.println("Arduino: AT+CMGR=1");
        gps.println("AT+CMGR=1");                       //When Arduino read the sign, send the "read" AT command to the module
        delay(10);
      }
    }
    else if (inchar=='L')
    {
      delay(10);
      inchar=gps.read(); 
      Serial.write(inchar);
      if (inchar=='H')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
      {
        Serial.println("SMS: LH");
        delay(10); 
        Serial.println("Arduino: AT+CMGD=1,4");
        gps.println("AT+CMGD=1,4");                    //Delete all message
        delay(500);
      }
      if (inchar=='L')                                    //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
      {
        Serial.println("SMS: LL");
        delay(10);
        Serial.println("Arduino: AT+CMGD=1,4");
        gps.println("AT+CMGD=1,4");                   //Delete all message
        delay(500);
      }
    }
  }
  /*
  if(Serial.available()>0)
  {
    inchar=Serial.read();
    lcd.print(inchar);
    if(inchar=='T')
    {
      delay(10);
      inchar=Serial.read(); 
      lcd.print(inchar);
      if (inchar=='I')                                      //When the GSM module get the message, it will display the sign '+CMTI "SM", 1' in the serial port
      {      
        delay(10);
        Serial.println("AT+CMGR=1");                       //When Arduino read the sign, send the "read" AT command to the module
        delay(10);
        lcd.clear();
      }
    }
    else if (inchar=='U')
    {
      delay(10);
      inchar=Serial.read(); 
      lcd.print(inchar);
      if (inchar=='N')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
      {
        delay(10);
        inchar=Serial.read(); 
        lcd.print(inchar);
        if (inchar=='L')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
        {
          delay(10);
          inchar=Serial.read(); 
          lcd.print(inchar);
          if (inchar=='O')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
          {
            delay(10);
            inchar=Serial.read(); 
            lcd.print(inchar);
            if (inchar=='C')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
            {
              delay(10);
              inchar=Serial.read(); 
              lcd.print(inchar);
              if (inchar=='K')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
              {
                delay(10);
                digitalWrite(ledpin,LOW);                         //Turn off led 
                delay(50);
                Serial.println("AT+CMGD=1");
                delay(500);     
                Serial.println("AT+CMGD=2");
                delay(500);
                Serial.println("AT+CMGD=3");
                delay(500);
                Serial.println("AT+CMGD=4");
                delay(500);
              }
            }
          }
        }
      }
    }
    else if (inchar=='L' || inchar=='l')                                    //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
    {
      delay(10);
      inchar=Serial.read(); 
      lcd.print(inchar);
      if (inchar=='O' || inchar=='o')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
      {
        delay(10);
        inchar=Serial.read(); 
        lcd.print(inchar);
        if (inchar=='C' || inchar=='c')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
        {
          delay(10);
          inchar=Serial.read(); 
          lcd.print(inchar);
          if (inchar=='K' || inchar=='k')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
          {
            delay(10);
            digitalWrite(ledpin,HIGH);                         //Turn off led 
            delay(50);
            Serial.println("AT+CMGD=1");     
            delay(500);
            Serial.println("AT+CMGD=2");
            delay(500);
            Serial.println("AT+CMGD=3");
            delay(500);
            Serial.println("AT+CMGD=4");                 //Delete all message
            delay(500);
          }
        }
      }
    }
}*/

void sendCommand(char command[]){
  delay(500);
  Serial.print("Arduino -> GPS: ");
  Serial.println(command);  // Send data to PC
  gps.println(command);  // Send data to GPS
  delay(500);
}
