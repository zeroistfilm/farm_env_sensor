

#include "WizFi360.h"
#include "PMS.h"

//#include <Adafruit_SSD1306.h> //OLED
#include "DFRobot_MICS.h" //가스센서
#include "DFRobot_EnvironmentalSensor.h" //환경센서
#include "DFRobot_OxygenSensor.h" //산소센서

/* Baudrate */
#define SERIAL_BAUDRATE   115200
#define SERIAL2_BAUDRATE  115200


//OLED
//#define SCREEN_WIDTH 128              // OLED 디스플레이의 가로 픽셀수
//#define SCREEN_HEIGHT 32
//#define OLED_RESET     -1
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//가스센서
#define CALIBRATION_TIME   0.1             // 추천값 3
#define COLLECT_NUMBER    10             // collect number, the collection range is 1-100.
DFRobot_MICS_I2C mics(&Wire1, 0x78);
//환경센서
DFRobot_EnvironmentalSensor environment(/*addr = */0x22, /*pWire = */&Wire1);
//산소센서
DFRobot_OxygenSensor Oxygen(0x73, &Wire1);
//미세먼지
PMS pms(Serial1);
PMS::DATA data;


/* Wi-Fi info */
char ssid[] = "yd2";       // your network SSID (name)
char pass[] = "123a123a11";   // your network password
int status = WL_IDLE_STATUS;  // the Wifi radio's status
int reqCount = 0;             // number of requests received
WiFiServer server(80);

void setup() {
  Wire1.setSDA(2);
Wire1.setSCL(3);

Wire1.begin(); // Wire communication begin

  Wire1.setClock(100000);

  
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize serial for debugging
  Serial.begin(SERIAL_BAUDRATE);
  //미세먼지
  Serial1.begin(9600);  
  pms.wakeUp();
  // initialize serial for WizFi360 module
  Serial2.begin(SERIAL2_BAUDRATE);

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
uint16_t pm1;
uint16_t pm2;
uint16_t pm10;

void loop() {
  if (pms.read(data))
  {
    
   pm1=data.PM_AE_UG_1_0;
   pm2=data.PM_AE_UG_2_5;
   pm10=data.PM_AE_UG_2_5;
  }

  
  WiFiClient client = server.available();
  if (client) {
    pms.read(data);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
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
        client.print(pm1);
        client.println(",");
        client.print("\"PM2.5\" : ");
        client.print(pm2);
        client.println(",");
        client.print("\"PM10\" : ");
        client.print(pm10);
        client.println("");
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
