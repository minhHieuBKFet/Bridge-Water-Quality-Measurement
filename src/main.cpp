#include <Arduino.h>
// Load Wi-Fi library
#include <WiFi.h>
#include "LoraSX1278.h"
#include "Datastructure.h"
// Replace with your network credentials
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

//PROGMEM char *str = "";

// Auxiliar variables to store the current output state
String sampleState = "off";
String getCSVFile = "off";

String nameFileSaveData = "";

String dateTime;
String latitude;
String longitude;
String depth;
String temp;
String DO_value;
float densityWater = 1.0;
void setup() {
  Serial.begin(9600);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
  String dateTime = "0";
  String latitude = "0";
  String longitude = "0";
  String depth = "0";
  String temp = "0";
  String DO_value = "0";
  LoraSX1278_Init();
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.println(SPIFFS.totalBytes());
  Serial.println(SPIFFS.usedBytes());
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
 
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (header.indexOf("GET /get-csv") >= 0) {
              getCSVFile = "on";
              String currentLine = "";                // make a String to hold incoming data from the client

              client.println("HTTP/1.1 200 OK");
              // client.println("Content-Type: text/csv");
              client.println("Content-Disposition: attachment; filename=" + nameFileSaveData + ".csv"); 
              client.println("Connection: close");
              client.print("Access-Control-Allow-Origin: *\r\n");
              String locationFileSaveData = "/" + nameFileSaveData +".csv";
              client.print("\r\n");
              File file = SPIFFS.open(locationFileSaveData, FILE_READ);
              if(file){
                while(file.available()){
                  client.write(file.read());
                }
                file.close();
                SPIFFS_checkOutOfMemory();
              }else{
                client.println("File not found.");
              }
              client.println();
              client.print("\r\n");

              break;
            }
            else{
              getCSVFile = "off";
            }

          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            // client.println("Content-Disposition: attachment; filename=" + nameFileSaveData + ".csv"); 
            client.println();
            if (header.indexOf("POST /sample/on") >= 0) {
              // Read data from the client
              String line = client.readStringUntil('\r');
              // Parse the data
              if (line.startsWith("dataInput=")) {
                String inputData = line.substring(10);
                densityWater = inputData.toFloat();
                Serial.println(densityWater);
              }
            }
            if (header.indexOf("GET /sample/on") >= 0) {              
              Serial.println("Sampling...");
              sampleState = "on";
              LoraSX1278_requestData(MEASUREMENT_DEVICE_ADDR, BRIDGE_DEVICE_ADDR, REQUEST_BYTE_1, REQUEST_BYTE_2, densityWater);
              uint8_t _errorCode = LoraSX1278_receiveData();
              if(_errorCode != ERROR_NONE){
                if(_errorCode == ERROR_LORA_SX1278_RECEIVE_TIMEOUT){
                  sampleState = "timeout";
                }
                else {sampleState = "failed";}
                // dateTime = "0000-00-00T00:00:00";
                // latitude = "0.000000";
                // longitude = "0.000000";
                // depth = "0.0";
                // temp = "0.0";
                // DO_value = "0";
              }
            }else if(header.indexOf("GET /sample/off") >= 0){
              sampleState = "off"; 
            }
            

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button{padding:16px 40px;background-color:#4CAF50;color:white;border:none;border-radius:5px;transition:background-color 0.3s;font-size: 30px;font-weight: bold;}button:hover{background-color:#45A049;}");
            client.println(".button2 {background-color: #555555;}.button2:hover{background-color:#3D3D3D;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Water Quality Measurement</h1>");
            client.println("<body><h2>SPARC LAB & COMPA STEM</h2>");
            
            // Display sensor value
            // client.println("<body><h4>Date Time: " + dateTime 
            //                     + "\tLatitude: "+ latitude 
            //                     +"\tLongitude: "+ longitude 
            //                     +"\tDepth: "+ depth + "mm"
            //                     +"\tTemp: "+ temp + "oC"
            //                     +"\tDO Value: "+ DO_value +"ug/L</h4>");
            // client.println("<body><h4>STT | \tDate Time | \tLatitude | \tLongitude | \tDepth(mm) | \tTemp(oC) | \tDO Value(ug/L)</h4>");
            displayDataList(client);
            client.println("</body></html>");
            client.println("<body><h4>Density of water (Kg/L): <input type = 'text' id='dataInput'></input></h4></body>");

            
            // Display current state 
            client.println("<p>Sample - State " + sampleState + "</p>");
            // If the sampleState is off, it displays the "Sample" button       
            if (sampleState=="off") {              
              client.println("<p><a href=\"/sample/on\"><button onclick='displayWait(); sendData();' class=\"button\">Sample</button> \
                              <script>function sendData(){ \
                              var input=document.getElementById('dataInput').value; \
                              var xhr=new XMLHttpRequest(); \
                              xhr.open('POST','/sample/on',true); \
                              xhr.setRequestHeader('Content-type','application/x-www-form-urlencoded'); \
                              xhr.send('dataInput='+input);} \
                              function displayWait(){ \
                              var button=document.getElementsByTagName('button')[0]; \
                              button.innerHTML='Waiting...';}</script></a></p>");
            } else {
              client.println("<p><a href=\"/sample/off\"><button class=\"button button2\">Reset</button></a></p>");
            } 
            if (getCSVFile == "off"){
              client.println("<p><a href=\"/get-csv\"><button class=\"button\">Dowload CSV File</button></a></p>");              
            }
            else{
              client.println("<p><a href=\"/sample/off\"><button class=\"button button2\">Back</button></a></p>");
            }
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}