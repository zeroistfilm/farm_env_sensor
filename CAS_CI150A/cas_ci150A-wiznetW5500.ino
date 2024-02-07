#include <Ethernet.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <ctime>


byte mac[] = {0x36, 0xD1, 0x1E, 0x9C, 0xA5, 0x01};
IPAddress ip(10, 4, 3, 30);
byte gateway[] = { 10, 4, 0, 1 };
byte subnet[] = { 255, 255, 0, 0 };

//IPAddress ip(192, 168, 0, 190);
//byte gateway[] = { 192, 168, 0, 99 };
//byte subnet[] = { 255, 255, 255, 0 };
byte dns[] = { 1, 1, 1, 1 };

EthernetServer server(80);
EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Ethernet.init(17);
  //Ethernet.begin(mac, ip);/
  //Ethernet.begin(mac); // DHCP
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  server.begin();
  Serial.begin(9600);
  Serial1.begin(9600);  // for RS232

   // NTP 클라이언트 시작
  timeClient.begin();
}

bool flag = false;
char receivedBuff[9]={0,};
float weight=0;

void loop()
{
  timeClient.update();

  // 현재 시간을 가져옵니다.
  //Serial.println(timeClient.getFormattedTime());

  static char receivedBuff[9] = {0};
  static int index = 0;

  while (Serial1.available() > 0) {
    char receivedChar = (char)Serial1.read();

    // 개행 문자가 도착하면 문자열을 출력합니다.
    if (receivedChar == '\n') {
      receivedBuff[index] = '\0'; // 종결 문자를 추가합니다.
      index = 0; // 인덱스를 초기화합니다.

      if (validateData(receivedBuff)){

        weight = parseBuff(receivedBuff);
        Serial.println(weight);

        break; // 루프를 빠져나갑니다.
      }

    } else if (index < 8) { // 버퍼 크기를 넘기지 않도록 합니다.
      receivedBuff[index++] = receivedChar;
    }
  }


  EthernetClient client = server.available();

    if (client) {
    String currentLine = "";
    String httpMethod = "";
    String httpUrl = "";
    String hexData = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (httpMethod == "GET" && httpUrl == "/reset") {
              Serial1.write(0x35);
              Serial1.write(0x5A);
              DynamicJsonDocument doc(1024);
              doc["message"] = "Reset command sent to Serial1";
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              serializeJson(doc, client);
              //reset();
            } else if (httpMethod == "GET" && httpUrl.startsWith("/hex")) {
              int pos = httpUrl.indexOf("message=");
              if (pos != -1) {
                      hexData = httpUrl.substring(pos + 8);
                      hexData.replace("%20", " "); // Replace URL-encoded spaces if necessary
                      hexData.replace("\"", ""); // Remove quotes if present
                      sendHexToSerial(hexData);

                      delay(500); // Wait for 1 second for the response to come in (adjust time as necessary)

                      // Initialize a String to hold the RS232 response
                      String rs232Response = "";

                      // Read the response if available
                      while (Serial1.available() > 0) {
                        char c = Serial1.read(); // Read the incoming byte
                        rs232Response += c; // Append the byte to the rs232Response String
                      }
                        DynamicJsonDocument doc(1024);
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: applicatio n/json");
                        client.println("Connection: close");
                        client.println();
                        doc["message"] = rs232Response;
                        doc["timestamp"] = formatEpochToKSTString();
                        doc["unixtime"] = timeClient.getEpochTime();
                        serializeJson(doc, client);
                    }
            }
            else if (httpMethod == "GET" && httpUrl == "/") {

              DynamicJsonDocument doc(1024);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              doc["weight"] = weight;
              doc["timestamp"] = formatEpochToKSTString();
              doc["unixtime"] = timeClient.getEpochTime();
              serializeJson(doc, client);
            }
            break;
          } else {
            if (currentLine.startsWith("GET") || currentLine.startsWith("POST")) {
              httpMethod = currentLine.substring(0, currentLine.indexOf(' '));
              httpUrl = currentLine.substring(currentLine.indexOf(' ') + 1, currentLine.lastIndexOf(' '));
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    delay(1);
    client.stop();
  }
}

String formatEpochToKSTString() {
    unsigned long epoch = timeClient.getEpochTime();

    time_t rawtime = (time_t)epoch;
    struct tm * ptm = gmtime(&rawtime);

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d%02d%02d_%02d:%02d:%02dI", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    return String(buffer);
}





// 01234567
// -   1.02


float parseBuff(char buff[]){
  char tmp[6];
  for (int i=1; i<8;i++){
     tmp[i-1] = buff[i];
  }
  float num= atof(tmp);
  if (buff[0]=='-'){
    num = num * -1;
  }
  return num;

}

bool validateData(char buff[]){
  if (strlen(buff)!=8) return false;
    for (int i=0; i<strlen(buff); i++){
        switch (i){
          case 0:
            if (isPlusMinus(buff[i])) continue;
          case 1 ... 4:
            if (isNum(buff[i])) continue;
          case 5:
            if (isDot(buff[i])) continue;
          case 6 ... 7:
             if (isNum(buff[i])) continue;
          default:
            return false;
        }
    }
  return true;
}

bool isPlusMinus(char c){
  switch (c){
    case 0x20:
      return true;
    case 0x2D:
      return true;
  }
  return false;
}
bool isDot(char c){
  switch (c){
    case 0x2E:
      return true;
    default:
      return false;
  }
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


void sendHexToSerial(String hexData) {
  // You need to implement this function to parse the hexData string
  // and send the bytes to Serial1. You can use sscanf or strtol to
  // convert each pair of hex characters to a byte and send it.
  for (unsigned int i = 0; i < hexData.length(); i += 2) {
    char hexByte[3];
    hexByte[0] = hexData[i];
    hexByte[1] = hexData[i + 1];
    hexByte[2] = '\0';
    byte b = (byte)strtol(hexByte, NULL, 16);
    Serial1.write(b);
  }
}