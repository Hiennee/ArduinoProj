//#include <DHT22.h>
#include <NTPtimeESP.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

#ifndef APSSID
#define APSSID "Nha Linh" // existing Wifi network
#define APPSK "13082005"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
ESP8266WebServer server(80);
MDNSResponder mdns;
DHT dht(D5, DHT11);

float tempVal, humidVal;

HTTPClient http;
WiFiClient wificlient;

NTPtime NTPch("ch.pool.ntp.org"); 
strDateTime dateTime;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connect to existing Wifi network...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //pinMode(pinLed1, OUTPUT);
  //pinMode(pinLed2, OUTPUT);
  //server.on("/", handleOnConnect);
  //server.on("/turnOnOffLed1", turnOnOffLed1);
  //server.on("/turnOnOffLed2", turnOnOffLed2);
  server.enableCORS(true);
  server.begin();
  Serial.println("HTTP server started");
  dht.begin();
  
  dateTime = NTPch.getNTPtime(7.0, 0);

  pinMode(D6, OUTPUT);
  digitalWrite(D6, HIGH);
  server.on("/", handleMainPage);
  server.on("/humidity", readHumidity);
  server.on("/teemperature", readTemperature);
}
void loop()
{
  server.handleClient();
  
  Serial.print("Độ ẩm: ");
  Serial.println(dht.readHumidity());
  Serial.print("Nhiệt độ: ");
  Serial.println(dht.readTemperature());
  delay(1000);

  //dateTime = NTPch.getNTPtime(7.0, 0);
  //NTPch.printDateTime(dateTime);
  readHumidity();
  delay(5000);
}

void handleMainPage()
{
  Serial.println("Accessing index page!");
  tempVal = 0.0;
  humidVal = 0.0;
  pinMode(D6, OUTPUT);
  digitalWrite(D6, LOW);
}

void readHumidity()
{
  humidVal = dht.readHumidity();
  dateTime = NTPch.getNTPtime(7.0, 0);

  while (!dateTime.valid)
  {
    dateTime = NTPch.getNTPtime(7.0, 0);
    delay(500);
    Serial.print(".");
  }
  
  StaticJsonDocument<500> json;
  json["Humidity"] = dht.readHumidity();
  json["Seconds"] = dateTime.second;
  json["Minutes"] = dateTime.minute;
  json["Hours"] = dateTime.hour;
  json["Date"] = dateTime.day;
  json["Month"] = dateTime.month;
  json["Year"] = dateTime.year;
  String doc;
  
  serializeJson(json, doc);
  Serial.println(doc);

  //Serial.println((bool)http.begin(wificlient, "192.168.13.109", 8080, "192.168.1.11/post"));
  http.begin(wificlient, "http://localhost:8080/post");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  //http.POST(doc);
  Serial.println(http.POST(doc));
  //Serial.print(http.getString());
  //http.POST()
  http.end();
}

void readTemperature()
{
  tempVal = dht.readTemperature();
}