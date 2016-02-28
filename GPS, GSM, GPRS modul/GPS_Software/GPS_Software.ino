char inchar;
void setup()
{    
  //Init the driver pins for GSM function
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  Serial.begin(9600);                                      //set the baud rate
  digitalWrite(5,HIGH);                                     //Output GSM Timing 
  delay(1500);
  digitalWrite(5,LOW);  
  digitalWrite(3,LOW);                                      //Enable the GSM mode
  digitalWrite(4,HIGH);                                     //Disable the GPS mode
  delay(2000);
  delay(5000);                                              //call ready
  delay(5000);
  Serial.println("AT+CMGD=1,4");                           //Delete all SMS in box
}
 
void loop()
{  
  if(Serial.available()>0)
  {
    inchar=Serial.read();
    if(inchar=='T')
    {
      delay(10);
      inchar=Serial.read(); 
      if (inchar=='I')                                      //When the GSM module get the message, it will display the sign '+CMTI "SM", 1' in the serial port
      {      
        delay(10);
        Serial.println("AT+CMGR=1");                       //When Arduino read the sign, send the "read" AT command to the module
        delay(10);
      }
    }
    else if (inchar=='U')
    {
      delay(10);
      inchar=Serial.read(); 
      if (inchar=='N')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
      {
        delay(10);
        inchar=Serial.read(); 
        if (inchar=='L')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
        {
          delay(10);
          inchar=Serial.read(); 
          if (inchar=='O')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
          {
            delay(10);
            inchar=Serial.read(); 
            if (inchar=='C')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
            {
              delay(10);
              inchar=Serial.read(); 
              if (inchar=='K')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
              {
                Serial.println("AT"); //Send AT command  
                delay(2000);
                Serial.println("AT");   
                delay(2000);
                //Send message
                Serial.println("AT+CMGF=1");
                delay(1000);
                Serial.println("AT+CMGS=\"776006865\"");//Change the receiver phone number
                delay(1000);
                Serial.print("Unlocked");//the message you want to send
                delay(1000);
                Serial.write(26);
                delay(50);        
                Serial.println("AT+CMGD=1,4");                    //Delete all message
                delay(500);
              }
            }
          }
        }
      }
    }
    else if (inchar=='L')
    {
      delay(10);
      inchar=Serial.read(); 
      if (inchar=='O')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
      {
        delay(10);
        inchar=Serial.read(); 
        if (inchar=='C')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
        {
          delay(10);
          inchar=Serial.read(); 
          if (inchar=='K')                                     //Thw SMS("LH") was display in the Serial port, and Arduino has recognize it.
          {
            Serial.println("AT"); //Send AT command  
            delay(2000);
            Serial.println("AT");   
            delay(2000);
            //Send message
            Serial.println("AT+CMGF=1");
            delay(1000);
            Serial.println("AT+CMGS=\"776006865\"");//Change the receiver phone number
            delay(1000);
            Serial.print("Locked");//the message you want to send
            delay(1000);
            Serial.write(26);
            delay(50);        
            Serial.println("AT+CMGD=1,4");                    //Delete all message
            delay(500);
          }
        }
      }
    }
  }
}
