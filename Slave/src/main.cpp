#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>

char temp[6];
SoftwareSerial s(5,6);
String reply;
int mos1, mos2, ldr1, ldr2;
String cmd;
int r1=8,r2=9;


void scan(){
  mos1 = map(analogRead(A0),1023,750,0,100); 
  //mos2 = (analogRead(A1)/1024)*100;
  //ldr1 = (analogRead(A2)/1024)*100;
  //ldr2 = (analogRead(A3)/1024)*100;
}

void status_reply(){
  StaticJsonDocument<3*JSON_OBJECT_SIZE(2)> doc;
  doc["p1"]["a"] = 100;
  doc["p1"]["m"] = mos1;
  doc["p2"]["a"] = 100;
  doc["p2"]["m"] = 100;
  serializeJson(doc, s);
  //JsonObject root = doc.to<JsonObject>();
  //reply = reply+'$';
}

/*void receiveEvent(){
  if(Wire.available()>0){
    cmd = Wire.readStringUntil('$');
  }
}

void requestEvent(){
  status_reply();
  for(int i=0; i<reply.length(); i++){
    Wire.write(reply[i]);
    Serial.print(reply[i]);}
  Serial.println();
}*/

void setup() {
  // put your setup code here, to run once:
  /*Wire.begin(8);
  Wire.onReceive(receiveEvent); // register receive event 
  Wire.onRequest(requestEvent); // register request event */
  Serial.begin(9600);
  s.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(s.available()>0){
    cmd = s.readStringUntil('$');
    //delay(500);
  }
  delay(300);
  //Serial.print("cdm = ");
  //Serial.println(cmd);
  //Serial.print("temp = ");
  //Serial.println(temp);
  //else{}
  if(cmd == "status"){
    scan();
    status_reply();
  }
  else if(cmd == "r1_ON"){
    digitalWrite(r1, LOW);
    delay(2000);
    digitalWrite(r1, HIGH);
    analogWrite(11, 200);
    delay(1000);
    analogWrite(11, LOW);
  }
  else if(cmd == "r2_ON"){
    digitalWrite(r2, LOW);
    delay(2000);
    digitalWrite(r2, HIGH);
  }
  else{
    s.write("received$");
    Serial.println(cmd);
  }
  cmd = "";
  delay(1000);
}