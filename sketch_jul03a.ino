#include "DFRobot_MICS.h"
#include "DFRobot_EnvironmentalSensor.h"
#define CALIBRATION_TIME   3      



#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_SSD1306.h>



byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 177);
EthernetServer server(80);

DFRobot_EnvironmentalSensor environment(/*addr = */0x22, /*pWire = */&Wire);
DFRobot_MICS_I2C mics(&Wire, 0x78);
#define SCREEN_WIDTH 128              // OLED 디스플레이의 가로 픽셀수
#define SCREEN_HEIGHT 32       
#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 








void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  Ethernet.init(17);  // WIZnet W5100S-EVB-Pico
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("IP : ");
  display.println(Ethernet.localIP());
  display.display();
  delay(1000);



  

  
  while(!mics.begin()){
    Serial.println("NO Deivces !");
    delay(1000);
  } Serial.println("Device connected successfully !");
  
  while(environment.begin() != 0){
    Serial.println(" Sensor initialize failed!!");
    delay(1000);
  }
  Serial.println(" Sensor  initialize success!!");


  uint8_t mode = mics.getPowerState();
  if(mode == SLEEP_MODE){
    mics.wakeUpMode();
    Serial.println("wake up sensor success!");
  }else{
    Serial.println("The sensor is wake up mode");
  }
  while(!mics.warmUpTime(CALIBRATION_TIME)){
    Serial.println("Please wait until the warm-up time is over!");
    delay(1000);
  }



}

void loop() {
  // put your main code here, to run repeatedly:

//
//  float ch4 = mics.getGasData(CH4);
//  float c2h5oh = mics.getGasData(C2H5OH);
//  float h2 = mics.getGasData(H2);
//  float nh3 = mics.getGasData(NH3);
//  float co = mics.getGasData(CO);
//  float no2 = mics.getGasData(NO2);
//  
//
//  
//  Serial.print(ch4);
//  Serial.println(" PPM ch4");
//
//  Serial.print(c2h5oh);
//  Serial.println(" PPM c2h5oh");
//
//  Serial.print(h2);
//  Serial.println(" PPM h2");
//
//  Serial.print(nh3);
//  Serial.println(" PPM nh3");
//
//  Serial.print(co);
//  Serial.println(" PPM co");
//
//  Serial.print(no2);
//  Serial.println(" PPM no2");
//  
//  float oxdata = mics.getGasData(OXDATA);
//  Serial.print(oxdata);
//  Serial.println(" oxdata");
//
//  float reddata = mics.getGasData(REDDATA);
//  Serial.print(reddata);
//  Serial.println(" reddata");
//  
//  Serial.println("-------------------------------");
//  Serial.print("Temp: ");
//  Serial.print(environment.getTemperature(TEMP_C));
//  Serial.println(" ℃");
//  Serial.print("Temp: ");
//  Serial.print(environment.getTemperature(TEMP_F));
//  Serial.println(" ℉");
//  Serial.print("Humidity: ");
//  Serial.print(environment.getHumidity());
//  Serial.println(" %");
//  Serial.print("Ultraviolet intensity: ");
//  Serial.print(environment.getUltravioletIntensity());
//  Serial.println(" mw/cm2");
//  Serial.print("LuminousIntensity: ");
//  Serial.print(environment.getLuminousIntensity());
//  Serial.println(" lx");
//  Serial.print("Atmospheric pressure: ");
//  Serial.print(environment.getAtmospherePressure(HPA));
//  Serial.println(" hpa");
//  Serial.print("Altitude: ");
//  Serial.print(environment.getElevation());
//  Serial.println(" m");
//  Serial.println("-------------------------------");
//
//  
//  delay(1000);

  EthernetClient client = server.available();
  if (client) {
    
    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          
          
          client.println("{");
          
          client.print("\"CH4\":");
          client.print(mics.getGasData(CH4));
          client.println(",");
          
          client.print("\"C2H5OH\":");
          client.print(mics.getGasData(C2H5OH));
          client.println(",");
          
          client.print("\"H2\":");
          client.print(mics.getGasData(H2));
          client.println(",");
          
          client.print("\"NH3\":");
          client.print(mics.getGasData(NH3));
          client.println(",");
          
          client.print("\"CO\":");
          client.print(mics.getGasData(CO));
          client.println(",");
          
          client.print("\"NO2\":");
          client.print(mics.getGasData(NO2));
          client.println(",");
          
          client.print("\"oxdata\" : ");
          client.print( mics.getGasData(OXDATA));
          client.println(",");
          
          client.print("\"reddata\" : ");  
          client.print( mics.getGasData(OXDATA));
          client.println(",");

        
          client.print("\"Temp\": ");
          client.print(environment.getTemperature(TEMP_C));
          client.println(",");
          
          client.print("\"Humidity\": ");
          client.print(environment.getHumidity());
          client.println(",");
          
          client.print("\"UV\": ");
          client.print(environment.getUltravioletIntensity());
          client.println(",");
          
          client.print("\"illuminance\": ");
          client.print(environment.getLuminousIntensity());
          client.println(",");
          
          client.print("\"AtmosphericPressure\": ");
          client.print(environment.getAtmospherePressure(HPA));
          client.println(",");

          client.print("\"Altitude\": ");
          client.print(environment.getElevation());
          
          
          
          client.println("}");
          
          
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }

}
