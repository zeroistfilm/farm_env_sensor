

#include "WizFi360.h"
#include <Wire.h> //include Wire.h library


/* Baudrate */
#define SERIAL_BAUDRATE   115200
#define SERIAL2_BAUDRATE  115200


/* Wi-Fi info */
char ssid[] = "yd2";       // your network SSID (name)
char pass[] = "123a123a11";   // your network password

int status = WL_IDLE_STATUS;  // the Wifi radio's status

int reqCount = 0;             // number of requests received

WiFiServer server(80);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
  // initialize serial for debugging
  Serial.begin(SERIAL_BAUDRATE);
  // initialize serial for WizFi360 module
  Serial2.begin(SERIAL2_BAUDRATE);


  Wire1.setSDA(2);
  Wire1.setSCL(3);
  
  Wire1.begin(); // Wire communication begin
  Wire1.setClock(100000);

  
  WiFi.init(&Serial2);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  
  // start the web server on port 80
  server.begin();
}

void loop() {
  
  byte error, address; //variable for error and I2C address
  int nDevices;

  nDevices = 0;


  
  WiFiClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is
        Serial.println("New client"); //blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response");
          
          // send a standard http response header
          // use \r\n instead of many println statements to speedup data send
          client.print(
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"  // the connection will be closed after completion of the response
            "Refresh: 20\r\n"        // refresh the page automatically every 20 sec
            "\r\n");
          client.print("<!DOCTYPE HTML>\r\n");
          client.print("<html>\r\n");
          client.print("<h1>Hello World!</h1>\r\n");
          client.print("Requests received: ");
          client.print(++reqCount);
          client.print("<br>\r\n");

            for (address = 1; address < 127; address++ )
            {
              // The i2c_scanner uses the return value of
              // the Write.endTransmisstion to see if
              // a device did acknowledge to the address.
              Wire1.beginTransmission(address);
              error = Wire1.endTransmission();
          
              if (error == 0)
              {
                Serial.print("I2C device found at address 0x");
                Serial.print(address, HEX);
                Serial.println("  !");
                
                client.print("I2C device found at address 0x");
                client.print(address, HEX);
                client.println("  !");
                
                nDevices++;
              }
            }
  
          
          client.print("<br>\r\n");
          client.print("</html>\r\n");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);

    // close the connection:
    client.stop();
    Serial.println("Client disconnected");
  }

}
