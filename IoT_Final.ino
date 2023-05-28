// Dan Koskiranta
// Date 9 May 2023
// IoT Project 2022-23: Intruder Detection System is a security system designed to help people to monitor their property.
// Some of the keypad code was borrowed from the following source https://esp32io.com/tutorials/esp32-keypad
// WiFi connection code was borrowed from the PowerPoint slides offered by Atlantic Technological University Galway-Mayo

#include <WiFi.h>
#include <WiFiClient.h>         // WiFiClient.h allows the ESP32 board to act as a client and to connect to other devices or servers over a WiFi network
#include <WebServer.h>
#include <Keypad.h>
#include <ESP32Servo.h>

const int SENSOR = 25;
const int LED = 26;
const int BUZZER = 13;

int sensorValue;
Servo servoMotor;

// Character to hold password input
char userInput[3];

char x = 0;

// Character for system deactivation
char character;

// Password for activation
char keypad_password[] = "123";

// Counter to check the password is correct
int passwordCount = 0;

// Character to hold key input
char keyPosition;

// Constants for row and column sizes
const int ROWS = 4;
const int COLUMNS = 4;

// Array to represent keys on keypad
char keys[ROWS][COLUMNS] = 
{
  {'1','2','3','F'},
  {'4','5','6','E'},
  {'7','8','9','D'},
  {'A','0','B','C'},
};

// Connections to ESP32
byte row_pins[ROWS] = {19,18,5,17};          // GPIO19, GPIO18, GPIO5, GPIO17 connect to the row pins
byte column_pins[COLUMNS] = {16,4,0,2};    

// Create keypad object
Keypad keypad_object = Keypad(makeKeymap(keys), row_pins, column_pins, ROWS, COLUMNS);
//https://esp32io.com/tutorials/esp32-keypad

const char* ssid = "iPhone"; // Enter the name of your phone hotspot here (usually just the name of you phone); must be 8 chars or less, no spaces or funny characters!
const char* password = "3otwnhquqsq6u"; // Enter the password you set on your phone for your hotspot; try enterinbg EXACTLY 8 chars, no spaces or funny characters!
// https://arduino.stackexchange.com/questions/57251/arduino-ide-c-11-raw-string-literal-have-issues-with-in-http-complier-t

// The server listens to this port: 80 is the default port for HTTP
// The web server will communicate with the web client via this port
WebServer server(80);

// Initialize a character pointer 'homePageHtmlPart1' with a raw string literal that contains an HTML document
// R indicates that the string is a raw string literal, and the sequence of '=' characters is a delimiter that specifies the start and end of the string
// Character pointer points to the first character of the string literal. With the character pointer you can manipulate strings of characters dynamically at runtime
char *homePageHtmlPart1 = R"=====(
<!DOCTYPE html>
<html>
<head>
<style>
 body {
  background: linear-gradient(to bottom, #ffffff 0%, #999966 100%);
}
.flex-container {
  display: flex;
  background-color: Red;
}

h1{
  padding: 2vh;
  text-align: center;
  font-size: 50px;
}
h2 {
  text-align: center;
  font-size: 45px;
}

.flex-container > div {
  background-color: #f1f1f1;
  margin: 10px;
  padding: 20px;
  font-size: 30px;
}
</style>
</head>
<body>

<h1>Intruder Detection System</h1>
<h2> Alarm System Activated </h2>
<br>
<br> 

<div class="flex-container">
  <div><span id='pir'></span></div>
  <div><span id='date'></span></div>
  <div><span id='time'></span></div>  
</div>

<script>
  async function updatePIR() {
   // alert('Waiting . . .  ')
    const Ares = await fetch("/getData")
  //  alert(Ares)
    const Adata = await Ares.text()
  //  alert(Adata)
    document.getElementById('pir').innerHTML = Adata
    let date = document.getElementById('date');
    let time = document.getElementById('time');
    let d = new Date();
    date.innerHTML = d.toLocaleDateString();
    time.innerHTML = d.toLocaleTimeString();
  }
  setInterval(updatePIR, 2000);
  
 </script>
 </body>
</html>
)=====";

// Interrupt function. The ESP32 will respond quickly with this function when a specific change in the pin level is detected
void IRAM_ATTR Interrupt() {
  digitalWrite(LED, HIGH);
  digitalWrite(BUZZER, HIGH);
}

String getPIR() {
  sensorValue = digitalRead(SENSOR);
  String retVal = "";
  if(sensorValue == HIGH) {
    retVal += "ALARM";                            
  }
  else if(sensorValue == LOW) {
    retVal += "  ";
  }
  return retVal;
 }

 // Handling function for a request to the server 
void handleRoot() {
  Serial.println("Sending home page");
  String message = homePageHtmlPart1;                                                                     
  server.send(200, "text/html", message);           // 200 means OK
}

// Function to send the PIR value to the web page
void handleTheFetch() {
  String info = getPIR();
  //Serial.println("Sending getPIR()");
  //Serial.println(info);
  server.send(200, "text/plain", info);           // The send() function sends a HTTP response to the client
 }


void setup() {
  // put your setup code here, to run once:
  pinMode(SENSOR, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);                   //Station mode. The ESP32 can connect to other networks
  WiFi.begin(ssid, password);           // Connect to WiFi network. Sets the SSID and password for the network to connect to
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());     // Get the boards IP address

  // Forward slash means that it will execute the function that is following. (/) is the root path/root URL.
  server.on("/", handleRoot);
  server.on("/getData", handleTheFetch);     // The server.on() method sets up a respond function to handle specific HTTP methods and URLs
  server.begin();                           // Start the server
  Serial.println("HTTP server started");
}

void loop() {
  // handleClient method on the server object handles the incoming HTTP requests. You have to call it repeteadly
  server.handleClient();                              // handleClient method generates a response to the web client
  delay(2);//allow the cpu to switch to other tasks

  Serial.println("\nPlease enter the pin: ");

  // Scan the keypad
  keyPosition = keypad_object.getKey();

  while(passwordCount != 3) {
    keyPosition = keypad_object.getKey();

    // If a key has been pressed
    if(keyPosition){
      Serial.println(keyPosition);
        
      for(x = 0; x < 3; x++){
        // A value from the user represents a key on the keypad
        userInput[x] = keyPosition;
        
        // If the value from the userInput array matches a value from the password array
        if(userInput[x] == keypad_password[x]){
          // Increment the password counter
          passwordCount++;
        }
      }
      
      // Correct Password. The password counter needs to be 3.
      if(passwordCount == 3){
        Serial.println("\nSystem Activated\n");
        delay(5000);
        servo();
      }
      else{
        Serial.println("\nIncorrect PIN. Try Again\n");
      }  
    }
  } 
}
