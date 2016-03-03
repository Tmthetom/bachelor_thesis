#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX: Arduino -> PC

byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS
int networkConnect = 5000;  // In real case, highly recommended bigger amount of time
int baudRate = 9600;  // Communication baud rate for GPS and PC
int common = 500;  // Delay between commands
int CtrlC = 26;  // ASCII code for Ctrl+C
int led = 13;  // Pin of signalization LED
char character = ' ';  // Empty character
String phone = "776006865";  // Default phone number
String string = "";  // Empty string
String SMS = "";  // Empty SMS content
boolean lock = false;  // Car is not locked
boolean readSMS = false;  // Can i read test of SMS now?

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
    character = gps.read();  // Read one character from shield serial
    string += character;  // Add character into string
    Serial.write(character);  // Send data to PC

    // If recieved command is completed
    if(character == '\n'){
      //Serial.println("Newline");
      string.trim();  // Delete all whitespace characters

      // New SMS indication and content of SMS
      if(string.equalsIgnoreCase("+CMTI: \"SM\",1")){
        sendCommand("AT+CMGR=1");  // Show content of SMS
      }
      else if(string.startsWith("+CMGR:")){
        readSMS = true;
      }
      else if(readSMS == true){
        Serial.print("Content of SMS: ");
        Serial.println(string);
        SMS = string;
        readSMS = false;
      }
      
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
