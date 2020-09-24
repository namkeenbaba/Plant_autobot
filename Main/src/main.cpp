#include <Arduino.h>
#include <ArduinoJson.h>
//--------------- Core NodeMCU libraries
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
//--------------- for communication with Slave
#include <SoftwareSerial.h>

const char* ssid = "TP-Link_81BC";
const char* password = "09870987";

const char* fingerprint = "BB DC 45 2A 07 E3 4A 71 33 40 32 DA BE 81 F7 72 6F 4A 2B 6B";
const char* BotToken = "784890187:AAGuBQx0ZY_Vo02Dwc35N54NNnEN4kxxSxY";
const char* host = "api.telegram.org";
const int ssl_port = 443;

boolean first_check = true;
String G_cmd, G_chat_id;
int update_id = 0;

/*
void scan(String cmd){
  if(cmd == "Status"){
    s.write("status$");
    StaticJsonDocument<146> doc;
    Serial.println("p2");
    while(s.available()<=0){}
    while(s.available()>0){
      DeserializationError error = deserializeJson(doc, s.readString());
      if(error){
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
      }
    }
    int mos1 = doc["p1"]["m"];
    int ldr1 = doc["p1"]["a"];
    int mos2 = doc["p2"]["m"];
    int ldr2 = doc["p2"]["a"];
    Serial.println("p3");
    String text_to_send = "plant1\nmoisture->"+String(mos1)+"% ambiance->"+String(ldr1)+"%\nplant2\nmoisture->"+String(mos2)+"%ambiance->"+String(ldr2)+"%";
    Serial.println(text_to_send);
  }
  else if(cmd == "R1_ON"){
    s.write("r1_ON$");
    String text_to_send = "Affirmative";
  }
  else if(cmd == "R2_ON"){
    s.write("r2_OFF$");
    String text_to_send = "Affirmative";
  }
  else{text_to_send = "FUCK YOU SEND ME SOMETHING I UNDERSTAND";}
}
*/
boolean Reply(WiFiClientSecure client){
  extern boolean first_check;
  extern String G_cmd, G_chat_id;
  HTTPClient http;
  http.begin(client, "https://api.telegram.org/bot784890187:AAGuBQx0ZY_Vo02Dwc35N54NNnEN4kxxSxY/getUpdates?offset=-1");
  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode > 0){
    StaticJsonDocument<JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6)+231> doc;
    auto error = deserializeJson(doc, http.getString());
    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return false;
    }
    //-------------------Reading from Json Buffer
    String cmd = doc["result"][0]["message"]["text"];
    G_cmd = cmd;
    update_id = doc["result"][0]["update_id"];
    String chat_id =  doc["result"][0]["message"]["chat"]["id"];
    G_chat_id = chat_id;
    Serial.print("cmd = ");
    Serial.println(cmd);
    Serial.print("chat_id = ");
    Serial.println(chat_id);
    http.end();
    Serial.print("update_id = ");
    Serial.println(update_id);
    //--------------------------validating if the update is new
  }
  if(first_check)
  return false;
  else{
    first_check = false;
    return true;
  }
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.println("Connecting...");
  }
  Serial.println("Connected.");
}

void loop(){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClientSecure client;
    client.setFingerprint(fingerprint);
    if(Reply(client)){
      Serial.println("recieved an update");
    }
  }
  //return 0;
}