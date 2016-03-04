#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX: Arduino -> PC

byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS
int networkConnect = 5000;  // In real case, highly recommended bigger amount of time
int baudRate = 9600;  // Communication baud rate for GPS and PC
int common = 500;  // Delay between commands
int CtrlC = 26;  // ASCII code for Ctrl+C
int led = 13;  // Pin of signalization LED
char character = ' ';  // Empty character
String ownerNumber = "776006865";  // Default phone number
String string = "";  // Empty string
String SMS = "";  // Empty SMS content
boolean lock = false;  // Car is not locked
boolean readSMS = false;  // Can i read content of SMS now?

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
    readGPS();
    
    if(character == '\n'){  // If recieved command is completed
      string.trim();  // Delete all whitespace characters
      
      recognize_SMS_New();
      recognize_SMS_Header();
      recognize_SMS_Content();
      
      string = "";  // Delete content of string
    }
  }

  // Communication: PC -> Shield
  if (Serial.available()){
    character = Serial.read();  // Read one character from PC

    // PO ODLADENI SMAZAT
    // PRO OBSAH STRINGU ZMACKNOUT '?'
    if (character == '?'){
      Serial.println("**********************");
      Serial.println(string);
      Serial.println("**********************");
    } else{
      gps.write(character);
    }
  }
}

void recognize_SMS_New(){
  if(string.equalsIgnoreCase("+CMTI: \"SM\",1")){  // If SMS indication
    sendCommand("AT+CMGR=1");  // Show content of SMS
  }
}

void recognize_SMS_Header(){
  if(string.startsWith("+CMGR:")){  // If SMS header
    readSMS = true;  // Set: Content is ready to read
    string = "";  // Delete content of string
  }
}

void recognize_SMS_Content(){
  if(string != "" && readSMS == true){  // If content is ready to read
    SMS = string;  // Move content into another string
    readSMS = false;  // Set: Content is not ready to read
    string = "";  // Delete content of string
    sendCommand("AT+CMGD=1,4");  // Delete all SMS
    
    sendSMS("Vase SMS: " + SMS);
  }
}

void readGPS(){
  character = gps.read();  // Read one character from shield serial
  string += character;  // Add character into string
  Serial.write(character);  // Send data to PC
}

void sendCommand(String command){
  gps.println(command);  // Send data to GPS
}

void sendSMS(String text){
  gps.println("AT+CMGF=1");  // Text mode
  delay(common);  // Time for respond
  gps.println("AT+CMGS=\"" + ownerNumber + "\"");  // Add phone number
  delay(common);  // Time for respond
  gps.print(text);  // Add text of SMS
  delay(common);  // Time for respond
  gps.write(CtrlC);  // Send SMS
}
