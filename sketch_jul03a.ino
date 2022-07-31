#include "DFRobot_MICS.h"
#include "DFRobot_EnvironmentalSensor.h"
#include "DFRobot_OxygenSensor.h"
#define CALIBRATION_TIME   0.1      
#define COLLECT_NUMBER    10             // collect number, the collection range is 1-100.
#define Oxygen_IICAddress ADDRESS_3


#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_SSD1306.h>

#define DLY_5000  5000
#define DLY_1000  1000




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
DFRobot_OxygenSensor Oxygen;


unsigned char pms[32]={0,};
void ClearPMS7003(){
  char temp;
  while (Serial1.available() ){                 // While nothing is received
    temp=Serial1.read();
   }

  delay(100);
}



void setup() {
  // put your setup code here, to run once:


   Serial1.setTX(12);
   Serial1.setRX(13);
   
   Serial1.begin(57600);
   Serial1.print("Serial1 is open");




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
  byte error, address; //variable for error and I2C address
  int nDevices=0;
  display.print("I2C : ");
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
         
      Serial.print("I2C device found at address 0x");
      if (address < 16)
      Serial.print("0");
      Serial.print(address, HEX);      
      Serial.println("  !");
 
      
      
      display.setCursor(30+nDevices*20,8);
      display.print(address, HEX);
      nDevices++;
    }
  }
  display.println("  !");

  
 

  display.display();
  delay(1000);

  
  
  while(!Oxygen.begin(0x73)) {
    Serial.println(" Oxygen I2c device number error !");
    delay(1000);
  } 

  
  while(!mics.begin()){
    Serial.println("mics NO Deivces !");
    delay(1000);
  } Serial.println("mics Device connected successfully !");
  
  while(environment.begin() != 0){
    Serial.println(" environment Sensor initialize failed!!");
    delay(1000);
  }
  Serial.println(" environment Sensor  initialize success!!");


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

 
  static int cur_time=0, pre_time=0;
  static int cur_time_1=0, pre_time_1=0;
   
   cur_time = millis();
  
  static byte temperature = 0;
  static byte humidity = 0;

  int chksum=0,res=0;;

  int PM_01;
  int PM_25;
  int PM_10;


    
// Get PMS7003 Dust sensor data from sofewareserial
//==============================================================================
    if(cur_time - pre_time_1 >= DLY_1000){
    // Update previous counter time.
    pre_time_1 = cur_time;
    

  if(Serial1.available()>=32){

  for(int j=0; j<32 ; j++){
    pms[j]=Serial1.read();
    if(j<30)
      chksum+=pms[j];
  }

  if(pms[30] != (unsigned char)(chksum>>8) 
      || pms[31]!= (unsigned char)(chksum) ){
      ClearPMS7003();
      Serial1.println("PMS7003 checksum fail");
  }else if(pms[0]!=0x42 || pms[1]!=0x4d ){
      ClearPMS7003();
      Serial1.println("PMS7003 STX fail");
  }else{
#if 1
    Serial1.print("Dust raw data debugging :  ");
    Serial1.print("1.0ug/m3:");
    Serial1.print(pms[10]);
    Serial1.print(pms[11]);
    Serial1.print("  ");
    Serial1.print("2.5ug/m3:");
    Serial1.print(pms[12]);
    Serial1.print(pms[13]);
    Serial1.print("  ");
    Serial1.print("2.5ug/m3:");
    Serial1.print(pms[14]);
    Serial1.println(pms[15]);
#endif

    PM_01=(pms[10]<<8) | pms[11];
    PM_25=(pms[12]<<8) | pms[13];
    PM_10=(pms[14]<<8) | pms[15];
    } 
  }

    }
   





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

          client.print("\"O2\" : ");  
          client.print(Oxygen.getOxygenData(COLLECT_NUMBER));
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
