#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX: Arduino -> PC

// Internal variables
byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS
int networkConnect = 1000;  // In real case, highly recommended bigger amount of time
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

// Debugging into software Serial
boolean debugging = true;

// SMS Commands
String commandSendCoordinates = "Kde jsi?";

// Steps to properly switche everything on
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
}
 
void loop(){ 

  if (Serial.available()){
    gps.write(Serial.read());
  }

  float lat = latitude();
  float lon = longitude();
    
  if(lat > -200 && lat < 200 && lon > -200 && lon < 200){
    validGPSCounter++;  // Add one valid GPS      
    if(validGPSCounter == 10){
      validCoordinates = "https://www.google.cz/maps?f=q&q=" + String(lat,5) + "," + String(lon,5) + "&z=16";
      validGPSCounter = 0;  // Reset valid GPS counter
      Serial.println(validCoordinates);
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

// If debugging variable true send info into serial
void sendReport(String text){
  if(debugging){
    Serial.println(text);
  }
}

// Due to easy change of serial name
void sendCommand(String command){
  gps.println(command);  // Send data to GPS
}

// Switch between GPS and GSM mods/networks
void changeMode(String text){
  
  // GSM to GPS
  if(text.equalsIgnoreCase("GPS")){
    sendCommand("AT+CGPSIPR=9600"); // Set data baudrate
    delay(common);
    sendCommand("AT+CGPSPWR=1"); // Turn on GPS power supply 
    delay(common);
    sendCommand("AT+CGPSRST=1"); // Reset GPS in autonomy mode
    delay(common);
    digitalWrite(gsmDriverPin[0],HIGH);  // Disable GSM
    digitalWrite(gsmDriverPin[1],LOW);  // Enable GPS
    delay(networkConnect);  // Connect into network
    moduleMode = "GPS";  // Change GSM to GPS
    sendReport("GPS Mode");  // Serial report

  // GPS to GSM 
  }else if(text.equalsIgnoreCase("GSM")){
    digitalWrite(gsmDriverPin[0],LOW);  // Enable GSM
    digitalWrite(gsmDriverPin[1],HIGH);  // Disable GPS
    delay(networkConnect);  // Connect into network
    delay(networkConnect);  // Connect into network
    delay(networkConnect);  // Connect into network
    moduleMode = "GSM";  // Change GPS to GSM
    sendReport("GSM Mode");  // Serial report
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
      val[i] = gps.read();  //get the data from the serial interface
      if(val[i]==value[i]){  //Match the protocol
        i++;
        if(i==6){
          i=0;
          return 1;  //break out after get the command
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
        lat[i] = gps.read();
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
        lon[i] = gps.read();
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
