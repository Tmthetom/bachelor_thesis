byte gsmDriverPin[3] = {3,4,5};  // Pins of GPS

void setup() {
  // Setup shield  
  for(int i = 0 ; i < 3; i++){pinMode(gsmDriverPin[i],OUTPUT);}
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(9600);

  // Start shield
  digitalWrite(gsmDriverPin[2],HIGH);  // Reset GSM timer
  delay(500);
  digitalWrite(gsmDriverPin[2],LOW);  // Enable GSM timer

  // Start GSM mode
  digitalWrite(gsmDriverPin[0],LOW);  // Enable GSM
  digitalWrite(gsmDriverPin[1],HIGH);  // Disable GPS
  
  // Connect into network
  delay(1000);  
  delay(1000);
  delay(1000);

  // GPS setup commands
  sendCommand("AT+CGPSIPR=9600"); // Set data baudrate
  delay(500);
  sendCommand("AT+CGPSPWR=1"); // Turn on GPS power supply 
  delay(500);
  sendCommand("AT+CGPSRST=1"); // Reset GPS in autonomy mode
  delay(500);

  // Start GPS mode
  digitalWrite(gsmDriverPin[0],HIGH);  // Enable GSM
  digitalWrite(gsmDriverPin[1],LOW);  // Disable GPS

  Serial.println("");
  Serial.println("---------- NEW COMMUNICATION WINDOW ----------");
  Serial.println("");
}

void loop() {
  
  float lat = latitude();
  float lon = longitude();

  Serial.print(lat, 5);
  Serial.print(", ");
  Serial.println(lon, 5);
}

// Due to easy change of serial name
void sendCommand(char command[]){
  for(int i = 0; i < sizeof(command) - 1; i++){
    Serial1.print(command[i]);
  }
  Serial1.println();  // Send data to GPS
}

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
  int i=0;
  char value[6]={'$','G','P','G','G','A'};  //match the gps protocol
  char val[6]={'0','0','0','0','0','0'};
  while(1){
    if(Serial1.available()){
      val[i] = Serial1.read();  //get the data from the serial interface
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
    if(Serial1.available()){
      val = Serial1.read();
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
      if(Serial1.available()){
        lat[i] = Serial1.read();
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
      if(Serial1.available()){
        lon[i] = Serial1.read();
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
/*
void function_for_nothing(){
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
  string.equalsIgnoreCase("SOME RANDOM TEXT FFROM MY HEAD");
}*/
