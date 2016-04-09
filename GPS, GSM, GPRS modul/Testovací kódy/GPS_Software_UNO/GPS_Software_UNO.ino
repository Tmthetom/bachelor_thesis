#include <SoftReset.h>  // For reseting arduino if NORMAL POWER DOWN

// Global variables
byte gsmDriverPin[3] = {3, 4, 5}; // Pins of GPS
const int networkConnect = 5000;  // In real case, highly recommended bigger amount of time
const int baudRate = 9600;  // Communication baud rate for GPS and PC
const int common = 2000;  // Delay between commands
const int CtrlC = 26;  // ASCII code for Ctrl+C
int ledGSM = 6;  // Pin of signalization GSM LED
int ledGPS = 7;  // Pin of signalization GPS LED
int validGPSCounter = 0;  // Counting how many valid GPS we acquired
char character = ' ';  // Empty character
float lat;  // Empty float for GPS coordinates
float lon;  // Empty float for GPS coordinates
String ownerNumber = "+420776006865";  // Owner phone number
String senderNumber = ownerNumber;  // Set sender number as default owner
String string = "";  // Empty string
String SMS = "";  // Empty SMS content
String moduleMode = "";  // Default is no mode
String validCoordinates = "";  // Empty string readSMSfor GPS coordinates
boolean lock = false;  // Car is not locked
boolean readSMS = false;  // Can i read content of SMS now? (After SMS header)
boolean smsQue = false;  // Wait with sending another SMS, until first will be sended
boolean stringComplete = false;  // Is string is complete?

// Debugging into software Serial
boolean debugging = false;

// SMS Commands
String commandSendCoordinates = "Kde jsi?";  // I want GPS coordinates

// SMS Responses
String reponseUnknownContent = "SMS ve spatnem tvaru:";  // If SMS content unknown
String reponseCoordinatesPreparing = "Poloha bude zaslana behem nekolika minut";  // Wait for SMS

// Steps to properly switch everything on
void setup() {

  // Setup shield
  for (int i = 0 ; i < 3; i++) {
    pinMode(gsmDriverPin[i], OUTPUT);  // All module pins set to OUTPUT
  }

  // Setup signalization
  pinMode(ledGSM, OUTPUT); // Setting of LED pin
  pinMode(ledGPS, OUTPUT); // Setting of LED pin
  digitalWrite(ledGSM, LOW);  // Turn GSM LED off
  digitalWrite(ledGPS, LOW);  // Turn GPS LED off

  // Open serial communications and wait for port to open:
  string.reserve(200);  // Memory allocation for incoming data
  //Serial.begin(baudRate);  // PC
  Serial.begin(baudRate);  // Module

  // Start shield
  digitalWrite(gsmDriverPin[2], HIGH); // Reset GSM timer
  delay(common);
  digitalWrite(gsmDriverPin[2], LOW); // Enable GSM timer

  // Get ready for communication and reading
  sendReport("");
  sendReport("---------- New communication window ----------");
  sendReport("");

  // Start selected module mode
  changeMode("GSM");  // Start GSM mode
  sendCommand("AT+CMGDA=\"DEL ALL\"");  // Delete all SMS
}

void loop() {

  // GSM (SMS) mode
  if (moduleMode.equalsIgnoreCase("GSM")) {
    if (Serial.available()) { // If data in serial buffer are ready
      character = Serial.read();  // Read one character from shield serial
      string += character;  // Add character into string
      //Serial.print(character);
      if (character == '\n') { // If recieved command is completed
        string.trim();  // Delete all whitespace characters

        recognizeERROR();  // If Normal power down
        recognizeSmsNew();  // If new SMS arrives send request for header and content
        recognizeSmsHeader();  // If SMS header finded
        recognizeSmsContent(); // Read SMS content right after header
        executeSmsContent();  // If SMS content OK, execute command in content

        string = "";  // Delete content of string
      }
    }
  }

  // GPS (coorinates) mode
  else if (moduleMode.equalsIgnoreCase("GPS")) {
    if (stringComplete) {
      if (string.startsWith("$GPGGA")) {
        parseCoordinates();  // Get latitude and longtitude
        countValidGpsCoordinates();  // Count numer of valid GPS coordinates
      }
      string = "";  // Clear content of string
      stringComplete = false;  // String readed
    }
  }
}

// If normal power down appears
void recognizeERROR() {
  if (string.startsWith("NORMAL")) { // If SMS indication
    sendReport("Normal power down");  // Serial report
    digitalWrite(ledGSM, HIGH);  // Turn GSM LED on
    digitalWrite(ledGPS, HIGH);  // Turn GPS LED on
    delay(networkConnect);  // Wait until reset
    delay(networkConnect);  // Wait until reset
    soft_restart();  // Restart Arduino
  }
}

// Recognize if GPS send NEW SMS indication
void recognizeSmsNew() {
  if (string.startsWith("+CMTI:") && smsQue == false) { // If SMS indication
    sendReport("New SMS arrives");  // Serial report
    sendCommand("AT+CMGR=1");  // Show content of SMS
  }
}

// Recognize if GPS send SMS header
void recognizeSmsHeader() {
  if (string.startsWith("+CMGR:")) { // If SMS header
    sendReport("SMS header reader");  // Serial report
    senderNumber = string.substring(21, 34);  // Parse sender number
    readSMS = true;  // Set: Content is ready to read
    string = "";  // Delete content of string
  }
}

// Read SMS content, because after header clearly must be content (hope so)
void recognizeSmsContent() {
  if (string != "" && readSMS == true) { // If content is ready to read
    sendReport("SMS Content reader");  // Serial report
    SMS = string;  // Move content into another string
    readSMS = false;  // Set: Content is not ready to read
    string = "";  // Delete content of string
    sendCommand("AT+CMGDA=\"DEL ALL\"");  // Delete all SMS
  }
}

// Find out, what user want in SMS
void executeSmsContent() {
  if (!SMS.equals("")) {  // Sometimes there are just \n

    // Send back GPS location
    if (SMS.equalsIgnoreCase(commandSendCoordinates)) {
      sendReport("Sending SMS with please wait report");  // Serial report
      smsQue = true;  // Set flag sending SMS
      sendSMS(reponseCoordinatesPreparing);  // Send response
      changeMode("GPS");  // Switch module to GPS
      SMS = "";  // Delete content of SMS
    }

    // Command not in table
    else {
      sendSMS(reponseUnknownContent + " " + SMS);  // Send responde
      SMS = "";  // Delete content of SMS
    }
  }
}

// For easy change of serial name
void sendCommand(String command) {
  Serial.println(command);  // Send data to GPS
}

// Send SMS into mobile number
void sendSMS(String text) {

  // Switch into SMS mod
  sendCommand("AT+CMGF=1");  // Text mode
  delay(common);  // Time for respond
  sendCommand("AT+CMGS=\"" + senderNumber + "\"");  // Set phone number
  delay(common);  // Time for respond

  // Send SMS
  Serial.print(text);  // Send text of SMS
  delay(common);  // Time for respond
  Serial.write(CtrlC);  // Send end of SMS
  delay(common);  // Time for respond
  delay(common);  // Time for respond
  delay(common);  // Time for respond

  smsQue = false;  // Set flag not sending SMS
}

// If debugging variable true send info into serial
void sendReport(String text) {
  if (debugging) {
    Serial.println(text);
  }
}

// Count numer of valid GPS coordinates
void countValidGpsCoordinates() {

  // Coordinates must be betwene +-200 and not 0
  if (lat > -200 && lat < 200 && lat != 0 && lon > -200 && lon < 200 && lon != 0) {
    validGPSCounter++;  // Add one valid GPS
    if (validGPSCounter == 20) {
      validCoordinates = "https://www.google.cz/maps?f=q&q=" + String(lat, 5) + "," + String(lon, 5) + "&z=16";
      sendReport(validCoordinates);
      validGPSCounter = 0;  // Reset valid GPS counter
      changeMode("GSM");  // Change mode to GSM
      sendReport("Sending SMS with coordinates");  // Serial report
      sendSMS(validCoordinates);  // Send coordinates into SMS
    }
  }
}

// Get latitude and longitude from GGA protocol in string
void parseCoordinates() {

  // Read raw coordinates data
  lat = string.substring(18, 29).toFloat(); // Positions based on appendix NMEA format tables,
  lon = string.substring(32, 44).toFloat(); // founded in SIM908 software datasheet

  // Fix coordinates thru algorithm
  lat = gpsCorrection(lat);
  lon = gpsCorrection(lon);

  // If debugging mode, send readed data
  if (debugging) {
    Serial.print(lat);
    Serial.print(", ");
    Serial.println(lon);
  }
}

// GPS coordinate correction algorithm
float gpsCorrection(float rawdata) {
  if (rawdata != 0) { // If coordinates are valid (0 when data are not numbers -> .toFloat())
    int deg = (int)(rawdata / 100);
    float minutes = rawdata - (deg * 100);
    float mindecimal = minutes / 60.0;
    float corrected = deg + mindecimal;

    return corrected;  // Return corrected coordinates
  }
  return rawdata;  // Return zero
}


// Switch between GPS and GSM mods/networks
void changeMode(String text) {

  // GSM to GPS
  if (text.equalsIgnoreCase("GPS")) {
    sendReport("GPS Mode");  // Serial report

    // For first start in GPS mode, we must be first in GSM mode
    if (moduleMode == "") { // Only if module started and not in mode
      changeMode("GSM");  // Switch to GSM
    }

    // GPS setup commands
    sendCommand("AT+CGPSIPR=9600"); // Set data baudrate
    delay(common);
    sendCommand("AT+CGPSPWR=1"); // Turn on GPS power supply
    delay(common);
    sendCommand("AT+CGPSRST=1"); // Reset GPS in autonomy mode
    delay(common);

    // Start GPS mode
    digitalWrite(gsmDriverPin[0], HIGH); // Enable GSM
    digitalWrite(gsmDriverPin[1], LOW); // Disable GPS
    delay(networkConnect);  // Connect into network
    moduleMode = "GPS";  // Change GSM to GPS mode
    digitalWrite(ledGSM, LOW);  // Turn GSM LED off
    digitalWrite(ledGPS, HIGH);  // Turn GPS LED on

    // GPS to GSM
  } else if (text.equalsIgnoreCase("GSM")) {
    sendReport("GSM Mode");  // Serial report
    digitalWrite(gsmDriverPin[0], LOW); // Enable GSM
    digitalWrite(gsmDriverPin[1], HIGH); // Disable GPS
    delay(networkConnect);  // Connect into network
    delay(networkConnect);  // Connect into network
    delay(networkConnect);  // Connect into network
    moduleMode = "GSM";  // Change GPS to GSM mode
    sendCommand("AT+CMGF=1");  // Text mode
    delay(common);  // Time for respond
    digitalWrite(ledGPS, LOW);  // Turn GPS LED off
    digitalWrite(ledGSM, HIGH);  // Turn GSM LED on
  }
}

// Handling hardware interruption
void serialEvent() {
  if (moduleMode.equalsIgnoreCase("GPS")) {
    while (Serial.available()) {  // Unil something in buffer
      char inChar = (char)Serial.read();  // Read new byte
      string += inChar;  // Add char into String
      if (inChar == '\n') {  // If newline char
        stringComplete = true;  // Set flag ON
        sendReport(string);  // Serial report
      }
    }
  }
}
