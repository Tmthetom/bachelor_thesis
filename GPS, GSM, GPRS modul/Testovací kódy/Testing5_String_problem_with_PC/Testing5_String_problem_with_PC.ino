#include <SoftwareSerial.h>
SoftwareSerial gps(10, 11);  // RX, TX: GPS -> Arduino

byte gsmDriverPin[3] = {3, 4, 5}; // Pins of GPS
String string = "";

void setup() {
  // Setup shield
  for (int i = 0 ; i < 3; i++) {
    pinMode(gsmDriverPin[i], OUTPUT);
  }

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  gps.begin(9600);

  // Start shield
  digitalWrite(gsmDriverPin[2], HIGH); // Reset GSM timer
  delay(500);
  digitalWrite(gsmDriverPin[2], LOW); // Enable GSM timer

  // Start GSM mode
  digitalWrite(gsmDriverPin[0], LOW); // Enable GSM
  digitalWrite(gsmDriverPin[1], HIGH); // Disable GPS

  // Connect into network
  delay(1000);
  delay(1000);
  delay(1000);

  // GPS setup commands
  gps.println("AT+CGPSIPR=9600"); // Set data baudrate
  delay(500);
  gps.println("AT+CGPSPWR=1"); // Turn on GPS power supply
  delay(500);
  gps.println("AT+CGPSRST=1"); // Reset GPS in autonomy mode
  delay(500);

  // Start GPS mode
  digitalWrite(gsmDriverPin[0], HIGH); // Enable GSM
  digitalWrite(gsmDriverPin[1], LOW); // Disable GPS

  Serial.println("");
  Serial.println("---------- NEW COMMUNICATION WINDOW ----------");
  Serial.println("");
}

void loop() {
  delay(1000);
}

void serialEvent() {
  Serial.print(gps.write());
}


