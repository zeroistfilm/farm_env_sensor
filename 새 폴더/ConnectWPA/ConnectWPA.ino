/*
 WizFi360 example: ConnectWPA
 
 This example connects to an encrypted WiFi network using a WizFi360 module.
 Then it prints the  MAC address of the WiFi shield, the IP address obtained
 and other network details.
*/

#include "WizFi360.h"

/* Baudrate */
#define SERIAL_BAUDRATE   115200
#define SERIAL2_BAUDRATE  115200


/* Wi-Fi info */
char ssid[] = "yd2";       // your network SSID (name)
char pass[] = "123a123a11";   // your network password

int status = WL_IDLE_STATUS;  // the Wifi radio's status

void setup() {
  // initialize serial for debugging
  Serial.begin(SERIAL_BAUDRATE);
  // initialize serial for WizFi360 module


  Serial2.begin(SERIAL2_BAUDRATE);

  // initialize WizFi360 module


  WiFi.init(&Serial2);


  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
}

void loop() {
  // print the network connection information every 10 seconds
  Serial.println();
  printCurrentNet();
  printWifiData();
  
  delay(1000);
}

void printWifiData() {
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to
  byte bssid[6];
  WiFi.BSSID(bssid);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[5], bssid[4], bssid[3], bssid[2], bssid[1], bssid[0]);
  Serial.print("BSSID: ");
  Serial.println(buf);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.println(rssi);
}
