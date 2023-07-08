#include "WizFi360.h"

/* Wi-Fi info */
char ssid[] = "yd2";       // your network SSID (name)
char pass[] = "123a123a11";   // your network password
int status = WL_IDLE_STATUS;  // the Wifi radio's status
int reqCount = 0;             // number of requests received
IPAddress ip(172, 30, 1, 100);
WiFiServer server(80);

#define SERIAL_BAUDRATE   115200
#define SERIAL2_BAUDRATE  115200

void setup() {
  
  
  Serial.begin(9600);
  Serial1.begin(9600);  
  Serial2.begin(SERIAL2_BAUDRATE);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.init(&Serial2);


  //Wifi
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



bool flag = false;
char receivedBuff[9]={0,};
float weight=0;

void loop() {
 if (Serial1.available()){

     if(flag != true){
              digitalWrite(LED_BUILTIN, HIGH);
              byte x = Serial1.read();
              if(x == 0x02)
              {
                   flag = true;                 //STX has arrived
              }
      }else{
                 byte m = Serial1.readBytesUntil(0x03, receivedBuff, 9); //0x03 is not saved
                 receivedBuff[m] = '\0'; //insert null-character for EOF;
                 Serial.println(receivedBuff);
                 if (validateData(receivedBuff)){
                    weight = parseBuff(receivedBuff);                    
                    Serial.println(weight);
                 }

                 memset(receivedBuff, 0x00, 9);
                 flag = false;
                 digitalWrite(LED_BUILTIN, LOW);
       }
    
  }
  
  WiFiClient client = server.available();
  if (client) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is
        Serial.println("New client"); //blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          Serial.println("Sending response");
          
          // send a standard http response header
          // use \r\n instead of many println statements to speedup data send



          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: keepalive");  // the connection will be closed after completion of the response
          
          client.println();
          
          
          client.println("{");
          
          client.print("\"weight\":");
          client.print(weight);
          client.println();
          
          client.println("}");

        
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

bool validateData(char buff[]){
  if (strlen(buff)!=8) return false;
    for (int i=0; i<strlen(buff); i++){
        switch (i){
          case 0:
            if (isNum(buff[i])) continue;
          case 1:
            if (isPlusMinus(buff[i])) continue;
          case 2 ... 3:
            if (isNum(buff[i])) continue;
          case 4:
            if (buff[i] == 0x2E) continue;
          case 5 ... 7:
             if (isNum(buff[i])) continue;
          default:
            return false;
        }
    }
  return true;
}

bool isNum(char c){
   switch (c){
    case 0x30 ... 0x39:
      return true;    
    case 0x20: //공백문자
      return true; 
    default:
      return false;
  }
  
}

bool isPlusMinus(char c){
  switch (c){
    case 0x2B:
      return true;
    case 0x2D:
      return true;
  }      
  return false;

}

float parseBuff(char buff[]){
  char tmp[6];
  for (int i=2; i<=8;i++){
     tmp[i-2] = buff[i];
  }
  float num= atof(tmp);
  if (buff[1]=='-'){
    num = num * -1;
  }
  return num;
}
