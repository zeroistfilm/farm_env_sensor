#include <SPI.h>
#include <Ethernet.h>

//#include <Adafruit_SSD1306.h> //OLED
#include "DFRobot_MICS.h" //가스센서
#include "DFRobot_EnvironmentalSensor.h" //환경센서
#include "DFRobot_OxygenSensor.h" //산소센서




//for PMS sensor

//초기화
//이더넷
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 177);
EthernetServer server(80);
//OLED
//#define SCREEN_WIDTH 128              // OLED 디스플레이의 가로 픽셀수
//#define SCREEN_HEIGHT 32
//#define OLED_RESET     -1
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//가스센서
#define CALIBRATION_TIME   3             // 추천값 3
#define COLLECT_NUMBER    10             // collect number, the collection range is 1-100.
DFRobot_MICS_I2C mics(&Wire, 0x78);
//환경센서
DFRobot_EnvironmentalSensor environment(/*addr = */0x22, /*pWire = */&Wire);
//산소센서
DFRobot_OxygenSensor Oxygen(0x73, &Wire);
//미세먼지
unsigned char pmsbytes[31];
#define HEAD_1 0x42
#define HEAD_2 0x4d
#define DLY_5000  5000
#define DLY_1000  1000
//RS232


//void displayWithMsg(String msg){
//  display.clearDisplay();
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0,0);
//  display.print("IP : ");
//  display.println(Ethernet.localIP());
//  byte error, address; //variable for error and I2C address
//  int nDevices=0;
//  display.print("I2C : ");
//  for (address = 1; address < 127; address++ )
//  {
//    // The i2c_scanner uses the return value of
//    // the Write.endTransmisstion to see if
//    // a device did acknowledge to the address.
//    Wire.beginTransmission(address);
//    error = Wire.endTransmission();
//
//    if (error == 0)
//    {
//
//      Serial.print("I2C device found at address 0x");
//      if (address < 16)
//      Serial.print("0");
//      Serial.print(address, HEX);
//      Serial.println("  !");
//      display.setCursor(30+nDevices*20,8);
//      display.print(address, HEX);
//      nDevices++;
//    }
//  }
//  display.setCursor(0,16);
//  display.print(msg);//CALIBRATION_TIME*60-count);
//  display.display();
//  
//}

void setup() {
//LED
 pinMode(LED_BUILTIN, OUTPUT);
 digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//PICO
  Serial.begin(9600);
//미세먼지
  Serial1.begin(9600);
//RS232
  Serial2.begin(9600);


//이더넷
  Ethernet.init(17);  // GPIO17 WIZnet W5100S-EVB-Pico
  //Ethernet.begin(mac, ip); //No dhcp
  Ethernet.begin(mac); //dhcp
  // Check for Ethernet hardware present
  while (true) {
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {    
      delay(1); // do nothing, no point running without Ethernet hardware
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    }else{
      break;
    }
    
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  server.begin();
//OLED
//  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//  display.clearDisplay();

//I2C, Ethernet Info Display
  
  Serial.println(Ethernet.localIP());
  



//환경센서
while(environment.begin() != 0){
    Serial.println(" environment Sensor initialize failed!!");
    delay(1000);
  }
  Serial.println(" environment Sensor  initialize success!!");
//산소센서
  while(!Oxygen.begin()) {
    Serial.println(" Oxygen I2c device number error !");
    delay(1000);
  }

//가스센서
 while(!mics.begin()){
    Serial.println("mics NO Deivces !");
    delay(1000);
  } Serial.println("mics Device connected successfully !");
    uint8_t mode = mics.getPowerState();
  if(mode == SLEEP_MODE){
    mics.wakeUpMode();
    Serial.println("wake up sensor success!");
  }else{
    Serial.println("The sensor is wake up mode");
  }
  int count = 0;
  while(!mics.warmUpTime(CALIBRATION_TIME)){
    Serial.println("Please wait until the warm-up time is over!");
//    displayWithMsg(String(CALIBRATION_TIME*60-count));
    delay(1000);
    count++;
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
  
  }



}

int PM1_0_val=0;
int PM2_5_val=0;
int PM10_val=0;
//이더넷
//OLED
//가스센서
//환경센서
//산소센서
//미세먼지
//RS232

void loop() {

  
//  displayWithMsg("working");   
  if(Serial1.available()>=31){
      
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  
  
    int i=0;

    //initialize first two bytes with 0x00
    pmsbytes[0] = 0x00;
    pmsbytes[1] = 0x00;
    
    for(i=0; i<31 ; i++){
      pmsbytes[i] = Serial1.read();

      //check first two bytes - HEAD_1 and HEAD_2, exit when it's not normal and read again from the start
      if( (i==0 && pmsbytes[0] != HEAD_1) || (i==1 && pmsbytes[1] != HEAD_2) ) {
        break;
      }
    }

    if(i>2) { // only when first two stream bytes are normal
      if(pmsbytes[29] == 0x00) {  // only when stream error code is 0
        PM1_0_val = (pmsbytes[10]<<8) | pmsbytes[11]; // pmsbytes[10]:HighByte + pmsbytes[11]:LowByte => two bytes
        PM2_5_val = (pmsbytes[12]<<8) | pmsbytes[13]; // pmsbytes[12]:HighByte + pmsbytes[13]:LowByte => two bytes
        PM10_val = (pmsbytes[14]<<8) | pmsbytes[15]; // pmsbytes[14]:HighByte + pmsbytes[15]:LowByte => two bytes
        
        Serial.print("PMS7003 sensor - PM1.0 : ");
        Serial.print(PM1_0_val);
        Serial.print(" ug/m^3,  PM2.5 : ");
        Serial.print(PM2_5_val);
        Serial.print(" ug/m^3,  PM10 : ");
        Serial.print(PM10_val);
        Serial.println(" ug/m^3");
      } else {
        Serial.println("Error skipped..");
      }
    } else {
      Serial.println("Bad stream format error");
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
                    client.println(",");

        client.print("\"PM1.0\" : ");
        client.print(PM1_0_val);
        client.println(",");
        client.print("\"PM2.5\" : ");
        client.print(PM2_5_val);
        client.println(",");
        client.print("\"PM10\" : ");
        client.print(PM10_val);
        client.println("");
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
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  }

}
