// Global variables
byte gsmDriverPin[3] = {3, 4, 5}; // Pins of GPS
int networkConnect = 1000;  // In real case, highly recommended bigger amount of time
int baudRate = 9600;  // Communication baud rate for GPS and PC
int common = 500;  // Delay between commands
int CtrlC = 26;  // ASCII code for Ctrl+C
int validGPSCounter = 0;  // Counting how many valid GPS we acquired
char character = ' ';  // Empty character
float lat;  // Empty float for GPS coordinates
float lon;  // Empty float for GPS coordinates
String ownerNumber = "776006865";  // Owner phone number
String string = "";  // Empty string
String SMS = "";  // Empty SMS content
String moduleMode = "";  // Default is no mode
String validCoordinates = "";  // Empty string readSMSfor GPS coordinates
boolean lock = false;  // Car is not locked
boolean readSMS = false;  // Can i read content of SMS now? (After SMS header)
boolean SMSQueue = false;  // Are we sending SMS?
boolean stringComplete = false;  // Is string is complete?

// Debugging into software Serial
boolean debugging = true;

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

  // Open serial communications and wait for port to open:
  string.reserve(200);  // Memory allocation for incoming data
  Serial.begin(baudRate);  // PC
  Serial1.begin(baudRate);  // Module

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
}

void loop() {

  // GSM (SMS) mode
  if (moduleMode.equalsIgnoreCase("GSM")) {
    if (Serial1.available()) { // If data in serial buffer are ready
      character = Serial1.read();  // Read one character from shield serial
      string += character;  // Add character into string

      if (character == '\n') { // If recieved command is completed
        string.trim();  // Delete all whitespace characters

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

// Recognize if GPS send NEW SMS indication
void recognizeSmsNew() {
  if (string.equalsIgnoreCase("+CMTI: \"SM\",1")) { // If SMS indication
    sendCommand("AT+CMGR=1");  // Show content of SMS
  }
}

// Recognize if GPS send SMS header
void recognizeSmsHeader() {
  if (string.startsWith("+CMGR:")) { // If SMS header
    readSMS = true;  // Set: Content is ready to read
    string = "";  // Delete content of string
  }
}

// Read SMS content, because after header clearly must be content (hope so)
void recognizeSmsContent() {
  if (string != "" && readSMS == true) { // If content is ready to read
    SMS = string;  // Move content into another string
    readSMS = false;  // Set: Content is not ready to read
    string = "";  // Delete content of string
    sendCommand("AT+CMGD=1,4");  // Delete all SMS
  }
}

// Find out, what user want in SMS
void executeSmsContent() {
  if (!SMS.equals("") && SMSQueue != true) {

    // Send back GPS location
    if (SMS.equalsIgnoreCase(commandSendCoordinates)) {
      sendReport("Odesilam SMS");  // Serial report
      SMSQueue = true;  // Mark we are sending SMS
      sendSMS(reponseCoordinatesPreparing);  // Send response
      changeMode("GPS");  // Switch module to GPS
      SMS = "";  // Delete content of SMS
    }

    // Command not in table
    else {
      SMSQueue = true;  // Mark we are sending SMS
      sendSMS(reponseUnknownContent + " " + SMS);  // Send responde
      SMS = "";  // Delete content of SMS
    }
  }
}

// For easy change of serial name
void sendCommand(String command) {
  Serial1.println(command);  // Send data to GPS
}

// Send SMS into mobile number
void sendSMS(String text) {

  // Switch into SMS mode
  sendCommand("AT+CMGF=1");  // Text mode
  delay(common);  // Time for respond
  sendCommand("AT+CMGS=\"" + ownerNumber + "\"");  // Set phone number
  delay(common);  // Time for respond

  // Send SMS
  Serial1.print(text);  // Send text of SMS
  delay(common);  // Time for respond
  Serial1.write(CtrlC);  // Send end of SMS

  SMSQueue = false;  // Mark ready for new SMS
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
  if(debugging){
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
    if(moduleMode == ""){  // Only if module started and not in mode
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

    // GPS to GSM
  } else if (text.equalsIgnoreCase("GSM")) {
    sendReport("GSM Mode");  // Serial report
    digitalWrite(gsmDriverPin[0], LOW); // Enable GSM
    digitalWrite(gsmDriverPin[1], HIGH); // Disable GPS
    delay(networkConnect);  // Connect into network
    delay(networkConnect);  // Connect into network
    delay(networkConnect);  // Connect into network
    moduleMode = "GSM";  // Change GPS to GSM mode
  }
}

// Handling hardware interruption
void serialEvent1() {
  if (moduleMode.equalsIgnoreCase("GPS")) {
    while (Serial1.available()) {  // Unil something in buffer
      char inChar = (char)Serial1.read();  // Read new byte
      string += inChar;  // Add char into String
      if (inChar == '\n') {  // If newline char
        stringComplete = true;  // Set flag ON
      }
    }
  }
}
