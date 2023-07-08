#include <SPI.h>
#include <Ethernet.h>

#include "DFRobot_AirQualitySensor.h"
#include "DFRobot_MultiGasSensor.h"





//for PMS sensor

//초기화
//이더넷
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 177);
EthernetServer server(80);

DFRobot_AirQualitySensor particle(&Wire ,0x19);

DFRobot_GAS_I2C gas(&Wire ,0x74);


void setup() {
//LED
 pinMode(LED_BUILTIN, OUTPUT);
 digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//PICO
  Serial.begin(9600);



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



  //air condition
  while(!particle.begin()){
    Serial.println("NO Deivces !");
    delay(1000);
  }
  Serial.println("sensor begin success!");
  uint8_t version = particle.gainVersion();
  Serial.print("version is : ");
  Serial.println(version);

  //NH3
  while(!gas.begin())
  {
    Serial.println("NO Deivces !");
    delay(1000);
  }
  Serial.println("The device is connected successfully!");

  gas.changeAcquireMode(gas.PASSIVITY);
  gas.setTempCompensation(gas.OFF);

}


void loop() {


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

          client.print("\"NH3\":");
          client.print(gas.readGasConcentrationPPM());
          client.println(",");

          client.print("\"0.3um\":");
          client.print(particle.gainParticleNum_Every0_1L(PARTICLENUM_0_3_UM_EVERY0_1L_AIR));
          client.println(",");

          client.print("\"0.5um\":");
          client.print(particle.gainParticleNum_Every0_1L(PARTICLENUM_0_5_UM_EVERY0_1L_AIR));
          client.println(",");

          client.print("\"1.0um\":");
          client.print(particle.gainParticleNum_Every0_1L(PARTICLENUM_1_0_UM_EVERY0_1L_AIR));
          client.println(",");

          client.print("\"2.5um\":");
          client.print(particle.gainParticleNum_Every0_1L(PARTICLENUM_2_5_UM_EVERY0_1L_AIR));
          client.println(",");

          client.print("\"5.0um\":");
          client.print(particle.gainParticleNum_Every0_1L(PARTICLENUM_5_0_UM_EVERY0_1L_AIR));
          client.println(",");
          client.print("\"10um\":");
          client.print(particle.gainParticleNum_Every0_1L(PARTICLENUM_10_UM_EVERY0_1L_AIR));





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
