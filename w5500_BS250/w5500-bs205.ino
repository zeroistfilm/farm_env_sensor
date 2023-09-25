#include <Ethernet.h>
#include <EthernetUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <ctime>


byte mac[] = {0x36, 0xD1, 0x1E, 0x9C, 0xA5, 0xDE};
IPAddress ip(192, 168, 10, 171);
//IPAddress ip(192, 168, 10, 190);
//byte gateway[] = { 192, 168, 10, 254 };
//byte subnet[] = { 255, 255, 255, 0 };
//byte dns[] = { 168, 126, 63, 1 };
EthernetServer server(80);
EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 32400, 60000);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Ethernet.init(17);
  Ethernet.begin(mac, ip);
  //Ethernet.begin(mac); // DHCP
  //Ethernet.begin(mac, ip, dns, gateway, subnet);
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

  EthernetClient client = server.available();

    if (client) {
    String currentLine = "";
    String httpMethod = "";
    String httpUrl = "";
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
            } else if (httpMethod == "GET" && httpUrl == "/") {
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

void reset() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
   }

}