#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

const char* ssid = "TP-Link_81BC";
const char* password = "09870987";
const char* fingerprint = "BB DC 45 2A 07 E3 4A 71 33 40 32 DA BE 81 F7 72 6F 4A 2B 6B";
const char* BotToken = "784890187:AAGuBQx0ZY_Vo02Dwc35N54NNnEN4kxxSxY";
const char* host = "api.telegram.org";
const int ssl_port = 443;
int update_id, previous_update_id = 0,mos1,ldr1,mos2,ldr2;
String G_cmd, G_chat_id, text_to_send;
boolean replyy;
SoftwareSerial s(D6, D5);

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
    mos1 = doc["p1"]["m"];
    ldr1 = doc["p1"]["a"];
    mos2 = doc["p2"]["m"];
    ldr2 = doc["p2"]["a"];
    Serial.println("p3");
    text_to_send = "plant1\nmoisture->"+String(mos1)+"% ambiance->"+String(ldr1)+"%\nplant2\nmoisture->"+String(mos2)+"%ambiance->"+String(ldr2)+"%";
    Serial.println(text_to_send);
  }
  else if(cmd == "R1_ON"){
    s.write("r1_ON$");
    text_to_send = "Affirmative";
  }
  else if(cmd == "R2_ON"){
    s.write("r2_OFF$");
    text_to_send = "Affirmative";
  }
  else{text_to_send = "FUCK YOU SEND ME SOMETHING I UNDERSTAND";}
}
//------------------------------function to send message.(when replyy = true i.e there is a new Update)
void reply(String cmd,String chat_id)
{
  Serial.println("p4");
  StaticJsonDocument<JSON_OBJECT_SIZE(2)+JSON_STRING_SIZE(85)> doc;
  doc["chat_id"] = chat_id;
	doc["text"] = text_to_send;
	String msg;
  serializeJson(doc, msg);
  Serial.println(msg);
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);
  Serial.println("p5");
  if(!client.connected()){
		client.connect(host, ssl_port);
  }
  HTTPClient http; //Object of class HTTPClient
  client.println("POST /bot"+String(BotToken)+"/sendMessage"+" HTTP/1.1");
	client.println("Host: api.telegram.org");
	client.println("Content-Type: application/json");
	client.println("Connection: close");
	client.print("Content-Length: ");
	client.println(msg.length());
	client.println();
  client.println(msg);
  String payload;
  Serial.println("next");
  while (client.connected()) {
		payload = client.readStringUntil('\n');
		if (payload == "\r"){
			break;
		 }
	  }
  payload = client.readStringUntil('\r');
  Serial.println("out");
  Serial.println(payload);
  text_to_send ="";
}
//----------------------------------function to get new updates
void get(){
  Serial.println("connected");
  WiFiClientSecure client;
  HTTPClient http;
  client.setFingerprint(fingerprint);
  http.begin(client, "https://api.telegram.org/bot784890187:AAGuBQx0ZY_Vo02Dwc35N54NNnEN4kxxSxY/getUpdates?offset=-1");
  int httpCode = http.GET();//------check if the request was sent correct
  Serial.println(httpCode);
  if (httpCode > 0){//--------------when the request was correct
    //Serial.println("into if");
    //const size_t bufferSize = JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6)+231;
    StaticJsonDocument<JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6)+231> doc;//creating Json for storing update
    auto error = deserializeJson(doc, http.getString());//parsing Update to Json buffer
    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }
    //-------------------------------Reading from Json buffer
    String cmd = doc["result"][0]["message"]["text"];
    G_cmd = cmd;
    update_id = doc["result"][0]["update_id"];
    String chat_id =  doc["result"][0]["message"]["chat"]["id"];
    G_chat_id =chat_id;
    Serial.print("cmd = ");
    Serial.println(cmd);
    Serial.print("chat_id = ");
    Serial.println(chat_id);
    http.end();
    Serial.print("update_id = ");
    Serial.println(update_id);
    //-------------------------------Checking it the update is new
    if(previous_update_id != update_id){
      replyy = true;
      previous_update_id = update_id;
      return;
    }
    else{
      replyy = false;
      Serial.println("No new Updates");
      return;
    }
  }
  else{
    replyy = false;
    return;
  }
}

void setup() 
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting...");
  }
  s.begin(9600);
}

void loop() 
{  
  if (WiFi.status() == WL_CONNECTED) 
  {
    get();
    if(replyy){
      scan(G_cmd);
      Serial.println("p1");
      reply(G_cmd, G_chat_id);
    }
    delay(20000);
  }
}