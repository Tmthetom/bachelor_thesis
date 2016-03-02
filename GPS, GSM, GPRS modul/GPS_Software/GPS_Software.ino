#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX: Arduino -> PC

byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS
int networkConnect = 2000;  // In real case, highly recommended bigger amount of time
int baudRate = 9600;  // Communication baud rate for GPS and PC
int common = 500;  // Delay between commands
int CtrlC = 26;  // ASCII code for Ctrl+C
int led = 13;  // Pin of signalization LED
char character = ' ';
String phone = "776006865";  // Default phone number
String string = "";  // Empty string
boolean lock = false;  // Car is not locked

void setup(){   
  // Setup shield  
  for(int i = 0 ; i < 3; i++){pinMode(gsmDriverPin[i],OUTPUT);}
  pinMode(led,OUTPUT);  // Setting of LED pin

  // Start shield
  digitalWrite(gsmDriverPin[2],HIGH);  // Reset GSM timer
  delay(common);
  digitalWrite(gsmDriverPin[2],LOW);  // Enable GSM timer
  digitalWrite(gsmDriverPin[0],LOW);  // Enable GSM
  digitalWrite(gsmDriverPin[1],HIGH);  // Disable GPS

  // Communication
  Serial.begin(baudRate);  // Connect with PC (Serial monitor)
  gps.begin(baudRate);  // Connect with GPS (GPS/GPRS/GSM Module v3.0)

  // Connect into GSM network
  delay(networkConnect);  // Time needed for connection

  // Connect into serial monitor
  Serial.println();
  Serial.println("-------- Nove komunikacni okno --------");
  Serial.println();

  // Initialization
  sendCommand("AT+CMGD=1,4");  // Delete all SMS
  digitalWrite(led,HIGH);  // GPS ready sign
}
 
void loop(){
  // Communication: Shield -> Arduino
  if(gps.available()){
    character = gps.read();
    string += character;
    Serial.write(character);

    if(character == '\n'){
      //Serial.println("Newline");
      string.trim();
      if(string.equalsIgnoreCase("+CMTI: \"SM\",1")){
        Serial.println("---- Nova SMS ----");
        sendCommand("AT+CMGR=1");  // Show content of SMS
      }
      string = "";
    }
  }

  // Communication: PC -> Shield
  if (Serial.available()){
    character = Serial.read();
    
    if (character == '?'){
      Serial.println("Obsah stringu:");
      Serial.println(string);
    } else{
      gps.write(character);
    }
  }

      /*
    string = readGPS();  // Read incoming GPS data

    // New SMS indication and content of SMS
    if(string.equalsIgnoreCase("+CMTI: \"SM\",1")){
      Serial.println("---- Nova SMS ----");
      sendCommand("AT+CMGR=1");  // Show conten.t of SMS

      //sendCommand("AT+CMGD=1,4");  // Delete all SMS
    }

    // Content of SMS
    if(string.equalsIgnoreCase("DOD")){
      
    }*/
}

/*
String readGPS(){
  String data;
  char character;
  while (gps.available()){  // Until are data in buffer
    character = gps.read();  // Read all data in buffer
    data += character;
  }
  Serial.println(data);  // Send data to PC
  data.trim();  // Delete any starting and ending whitespaces
  return data;
}
*/

void sendCommand(String command){
  gps.println(command);  // Send data to GPS
}

void sendSMS(String sms){
  Serial.println("AT+CMGF=1");  // Text mode
  delay(common);
  Serial.print("AT+CMGS=\"776006865\"");  // Add phone number
  delay(common);
  Serial.print(sms);  // Add text of SMS
  delay(common);
  Serial.write(CtrlC);  // Send SMS
}
