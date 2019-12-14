//#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
//#include <SPI.h>

//WiFiClient client;
//String request_string;
//const char* host = "";
//HTTPClient http;
int lampu = D5;
//
//#define WIFI_SSID "Xperia Z5 Dual"
//#define WIFI_PASSWORD "fardhanimusvian"
 
void setup(){
  pinMode(lampu, OUTPUT);
//  //connect wifi
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  Serial.print("connecting");
//  while(WiFi.status() != WL_CONNECTED){
//    Serial.print(".");
//    delay(500);
//  }
//  Serial.println("connected");
  Serial.begin(9600);
}
 
void loop() {
  digitalWrite(lampu, HIGH);
  Serial.println("hidup");
  delay(1000); //milisecond
//  delay(1000); //milisecond
  digitalWrite(lampu, LOW);
  Serial.println("mati");
  delay(1000);
//  delay(1000); //milisecond
}
