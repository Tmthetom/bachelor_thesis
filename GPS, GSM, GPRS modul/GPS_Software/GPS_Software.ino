#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX: Arduino -> PC

byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS
int networkConnect = 5000;  // In real case, highly recommended bigger amount of time
int baudRate = 9600;  // Communication baud rate for GPS and PC
int common = 500;  // Delay between commands
int CtrlC = 26;  // ASCII code for Ctrl+C
int led = 13;  // Pin of signalization LED
int validGPSCounter = 0;  // Counting how many valid GPS we acquired
char character = ' ';  // Empty character
float zemDelka;  // Empty float for GPS coordinates
float zemSirka;  // Empty float for GPS coordinates
String ownerNumber = "776006865";  // Owner phone number
String string = "";  // Empty string
String SMS = "";  // Empty SMS content
String moduleMode = "";  // Default is no mode
String validCoordinates = "";  // Empty string for GPS coordinates
boolean lock = false;  // Car is not locked
boolean readSMS = false;  // Can i read content of SMS now?
boolean SMSQueue = false;  // Are we sending SMS?

// SMS Commands
String commandSendCoordinates = "Kde jsi?";

void setup(){   
  // Setup shield  
  for(int i = 0 ; i < 3; i++){pinMode(gsmDriverPin[i],OUTPUT);}
  pinMode(led,OUTPUT);  // Setting of LED pin

  // Start shield
  digitalWrite(gsmDriverPin[2],HIGH);  // Reset GSM timer
  delay(common);
  digitalWrite(gsmDriverPin[2],LOW);  // Enable GSM timer

  // Communication start
  Serial.begin(baudRate);  // Connect with PC (Serial monitor)
  gps.begin(baudRate);  // Connect with GPS (GPS/GPRS/GSM Module v3.0)
  
  // Connect into serial monitor
  Serial.println();
  Serial.println("-------- Nove komunikacni okno --------");
  Serial.println();

  changeMode("GSM");  // Change module mode to GSM
  changeMode("GPS");  // Change module mode to GPS
/*
  // Initialization
  sendCommand("AT+CMGD=1,4");  // Delete all SMS
  digitalWrite(led,HIGH);  // GPS ready sign*/
}
 
void loop(){
  if(moduleMode.equalsIgnoreCase("GSM")){
    // Communication: Shield -> Arduino
    if(gps.available()){
      readGPSbyChars(true);
      
      if(character == '\n'){  // If recieved command is completed
        string.trim();  // Delete all whitespace characters
        
        recognize_SMS_New();
        recognize_SMS_Header();
        recognize_SMS_Content();
        execute_SMS_Content();
        
        string = "";  // Delete content of string
      }
    }
  }else if (moduleMode.equalsIgnoreCase("GPS")){
    Serial.println("Reading latitude");
    float lat = latitude();
    Serial.println("Reading longitude");
    float lon = longitude();
    
    if(lat > -200 && lat < 200 && lon > -200 && lon < 200){
      validGPSCounter++;  // Add one valid GPS      
      if(validGPSCounter == 10){
        validCoordinates = "https://www.google.cz/maps?f=q&q=" + String(lat,5) + "," + String(lon,5) + "&z=16";
        validGPSCounter = 0;  // Reset valid GPS counter
        //changeMode("GSM");  // Change mode to GSM
        
        //sendSMS(validCoordinates);
      }else{
        Serial.print(lat,5);
        Serial.print(", ");
        Serial.println(lon,5);
      }
    }else{
      Serial.print(lat,5);
      Serial.print(", ");
      Serial.println(lon,5);
    }
  }

  // Communication: PC -> Shield
  if (Serial.available()){
    gps.write(Serial.read());
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
  }
}

void execute_SMS_Content(){
  if(!SMS.equals("") && SMSQueue != true){
    if(SMS.equalsIgnoreCase(commandSendCoordinates)){
      Serial.println("Odesilam SMS");
      SMSQueue = true;  // Mark we are sending SMS
      sendSMS("Poloha bude zaslana behem nekolika minut");  // Send response
      changeMode("GPS");  // Switch module to GPS
      SMS = "";  // Delete content of SMS
    }else{
      SMSQueue = true;  // Mark we are sending SMS
      sendSMS("SMS ve spatnem tvaru: " + SMS);  // Send responde
      SMS = "";  // Delete content of SMS
    }
  }
}

void readGPSbyChars(boolean withReport){
  character = gps.read();  // Read one character from shield serial
  string += character;  // Add character into string
  if(withReport){
    Serial.write(character);  // Send data to PC
  }
}

void readGPS(boolean withReport){
  character = ' ';

  while(character != '\n'){
    if (gps.available()){
      character = gps.read();  // Read one character from shield serial
      string += character;  // Add character into string
      if(withReport){
        Serial.write(character);  // Send data to PC
      }
    }
  }
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
  SMSQueue = false;  // Mark ready for new SMS
}

int changeMode(String text){
  if(text.equalsIgnoreCase("GPS")){  // GSM to GPS
    sendCommand("AT+CGPSPWR=1");  // Turn on GPS power supply
    delay(common);
    sendCommand("AT+CGPSRST=1");  // Reset GPS in autonomy mode
    delay(common);
    Serial.flush();  // Clean content of Arduino serial
    gps.flush();  // Clean content of GPS serial
    digitalWrite(gsmDriverPin[0],HIGH);  // Disable GSM
    digitalWrite(gsmDriverPin[1],LOW);  // Enable GPS
    delay(networkConnect);  // Connect into network
    moduleMode = "GPS";  // Change GSM to GPS
    Serial.println("GPS Mode");
    //string = "";
    while(1){
      readGPSbyChars(true);
      /*
      if(character == '$'){
        Serial.print("$");
        readGPSbyChars(false);
        if(character == 'G'){
          Serial.print("G");
          readGPSbyChars(false);
          if(character == 'P'){
            readGPSbyChars(false);
            Serial.print("P");
            if(character == 'Z'){
              Serial.println("IN");
              readGPS(false);
              return 1;
            }
          }
        }
      }
      
      /*
      if (gps.available()){
        readGPSbyChars(true);
        if (character == '\n'){
          Serial.println("Kontroluji " + string);
          readGPS(false);
          string.trim();
          if (string.startsWith("$GPVTG")){
            Serial.println("GOOD");
            return 1;
          }
        }
        //Serial.write(gps.read());  // Send data to PC
      }*/
    }
  }else if(text.equalsIgnoreCase("GSM")){  // GPS to GSM 
    Serial.flush();  // Clean content of Arduino serial
    gps.flush();  // Clean content of GPS serial
    digitalWrite(gsmDriverPin[0],LOW);  // Enable GSM
    digitalWrite(gsmDriverPin[1],HIGH);  // Disable GPS
    delay(networkConnect);  // Connect into network
    moduleMode = "GSM";  // Change GPS to GSM
    Serial.println("GSM mode");
    return 1;
  }
}

// START OF CODE FROM GPS MODULE WEBSITE
double Datatransfer(char *data_buf,char num){                                                                    
  double temp=0.0;                           
  unsigned char i,j;
  if(data_buf[0]=='-'){
    i=1;
    while(data_buf[i]!='.')  //process the data array
      temp=temp*10+(data_buf[i++]-0x30);
    for(j=0;j<num;j++)
      temp=temp*10+(data_buf[++i]-0x30);
    
    for(j=0;j<num;j++)
      temp=temp/10;  //convert the int type to the float type
    temp=0-temp;  //convert to the negative numbe
  }
  else{
    i=0;
    while(data_buf[i]!='.')
      temp=temp*10+(data_buf[i++]-0x30);
    for(j=0;j<num;j++)
      temp=temp*10+(data_buf[++i]-0x30);
    for(j=0;j<num;j++)
      temp=temp/10 ;
  }
  return temp;
}
 
char ID(){  //Match the ID commands
  char i=0;
  char value[6]={'$','G','P','G','G','A'};  //match the gps protocol
  char val[6]={'0','0','0','0','0','0'};
  while(1){
    if(gps.available()){
      readGPSbyChars(true);
      val[i] = character;
      //val[i] = gps.read();  //get the data from the serial interface
      if(val[i]==value[i]){  //Match the protocol
        i++;
        if(i==6){
          i=0;
          return 1;  //break out after get the command
          Serial.println("ID OK");
        }
      }
      else
        i=0;
    }
  } 
}
 
void comma(char num){  //get ','
  char val;
  char count=0;  //count the number of ','
  
  while(1){
    if(gps.available()){
      val = gps.read();
      if(val==',')
        count++;
    }
    if(count==num)  //if the command is right, run return
      return;
  }
}

double decimalgps(double rawdata){
  int degrees = (int)(rawdata / 100);
  double minutes = rawdata - (degrees*100);
  double mindecimal = minutes / 60.0;
  double total = degrees + mindecimal;

  return total;
}

float latitude(){  //get latitude
  char i;
  char lat[10]={'0','0','0','0','0','0','0','0','0','0'};
  if(ID()){
    comma(2);
    while(1){
      if(gps.available()){
        readGPSbyChars(true);
        lat[i] = character;
        //lat[i] = gps.read();
        i++;
      }
      if(i==10){
        i=0;
        double newlat =  Datatransfer(lat,5);
        float corrected = decimalgps(newlat);
        return corrected;
      }  
    }
  }
}

float longitude()  //get longitude
{
  char i;
  char lon[11]={'0','0','0','0','0','0','0','0','0','0','0'};
 
  if(ID()){
    comma(4);
    while(1){
      if(gps.available()){
        readGPSbyChars(true);
        lon[i] = character;
        //lon[i] = gps.read();
        i++;
      }
      if(i==11){
        i=0;
        double newlon = Datatransfer(lon,5);
        float corrected = decimalgps(newlon);
        return corrected;
      }  
    }
  }
}

// END OF CODE FROM GPS MODULE WEBSITE
