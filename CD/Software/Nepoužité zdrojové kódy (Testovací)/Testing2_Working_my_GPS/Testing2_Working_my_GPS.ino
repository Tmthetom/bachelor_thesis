#include <SoftwareSerial.h>
SoftwareSerial gps(6, 7);  // RX, TX: Arduino -> PC

byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS
int networkConnect = 5000;  // In real case, highly recommended bigger amount of time
int baudRate = 9600;  // Communication baud rate for GPS and PC
int common = 500;  // Delay between commands

int validGPSCounter = 0;
String validCoordinates;

void setup(){
  // Setup shield  
  for(int i = 0 ; i < 3; i++){pinMode(gsmDriverPin[i],OUTPUT);}
  
  // Start shield
  digitalWrite(gsmDriverPin[2],HIGH);  // Reset GSM timer
  delay(common);
  digitalWrite(gsmDriverPin[2],LOW);  // Enable GSM timer

  digitalWrite(3,LOW);//Enable the GSM mode
  digitalWrite(4,HIGH);//Disable the GPS mode

   // Communication start
  Serial.begin(baudRate);  // Connect with PC (Serial monitor)
  gps.begin(baudRate);  // Connect with GPS (GPS/GPRS/GSM Module v3.0)

  // Connect into serial monitor
  Serial.println();
  Serial.println("-------- Nove komunikacni okno --------");
  Serial.println();

  delay(1000);//call ready
  delay(1000);
  delay(1000);

  gps.println("AT+CGPSIPR=9600"); //Send AT command  
  delay(500);
  gps.println("AT+CGPSPWR=1"); //Send AT command  
  delay(500);
  gps.println("AT+CGPSRST=1"); //Send AT command  
  delay(500);

  digitalWrite(3,HIGH);//Enable the GSM mode
  digitalWrite(4,LOW);//Disable the GPS mode
 }
 void loop(){ /*
    if(gps.available()){
      Serial.write(gps.read());
    }*/
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
