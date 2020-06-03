/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/control-esp32-esp8266-gpios-from-anywhere/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define PIN 22

#define BUTTONPIN 23
#define ONBOARD_LED  2

const char* ssid = "Sue Associates";
const char* password = "Do you know the way to mayan warrior?";
bool onSuana;
//Your IP address or domain name with URL path
const char* serverName = "http://us-central1-sue-associates.cloudfunctions.net/sauna";

// Update interval time set to 5 seconds
const long interval = 1000;
unsigned long previousMillis = 0;
int buttonState = 0;
int lastButtonState = 0; 

String outputsState;




void setup() {
  Serial.begin(115200);
  pinMode(ONBOARD_LED,OUTPUT);
  pinMode(BUTTONPIN, INPUT);
  pinMode(PIN, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(ONBOARD_LED,HIGH);
  delay(200);
  digitalWrite(ONBOARD_LED,LOW);
}

void loop() {
  unsigned long currentMillis = millis();

//  Serial.println("before time if");
  
  if(currentMillis - previousMillis >= interval) {
//      Serial.println("beginning of time if");
     // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED ){ 
      Serial.println("wifi connected");
      outputsState = httpGETRequest(serverName);
//      Serial.println("get request converted to string");
      Serial.println(outputsState);
      JSONVar myObject = JSON.parse(outputsState);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
//      Serial.println("exited time if");
//      Serial.print("JSON object = ");
//      Serial.println(myObject);
        Serial.println(myObject["on"]);

        String isOn = JSON.stringify(myObject["on"]);

        Serial.println("STRING IS ON IS " + isOn);
        
      // myObject.keys() can be used to get an array of all the keys in the object
      if (isOn == "true") {
        onSuana = true;
      }
      else {
        onSuana = false;
      }
      if (onSuana) {
        digitalWrite(ONBOARD_LED,HIGH);
        Serial.println("on suana is true");
        digitalWrite(PIN, HIGH);

      }

      else {
        Serial.println("on suana is false");
        digitalWrite(PIN, LOW);
        digitalWrite(ONBOARD_LED,LOW);
      }
    
//      for (int i = 0; i < keys.length(); i++) {
//        JSONVar value = myObject[keys[i]];
//        Serial.print("GPIO: ");
//        Serial.print(keys[i]);
//        Serial.print(" - SET to: ");
//        Serial.println(value);
//        pinMode(atoi(keys[i]), OUTPUT);
//        digitalWrite(atoi(keys[i]), atoi(value));
//      }
      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }

   buttonState = digitalRead(BUTTONPIN);

   
   if (buttonState != lastButtonState) {
    Serial.println("BUTTON IS PUSHED");
    if (buttonState == HIGH) {
      if (onSuana == false) {
        onSuana = true;
        httpPOSTRequest(serverName, true);
      }
      else {
        onSuana = false;
        httpPOSTRequest(serverName, false);
      }
   delay(200);
    }

     if (onSuana) {
      digitalWrite(PIN, HIGH);
        }
     else {
      digitalWrite(PIN, LOW);
        }
    }  
}

void httpPOSTRequest(const char* serverName, bool onOff) {
  HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
//      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
//      String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
      // Send HTTP POST request
//      int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      if (onOff) {
        int httpResponseCode = http.POST("{\"on\":true}"); //http.POST(“{\”api_key\”:\”tPmAT5Ab3j7F9\”,\”sensor\”:\”BME280\”,\”value1\”:variable,\”value2\”:variable,\”value3\”:variable}”);  
      }
      else {
        int httpResponseCode = http.POST("{\"on\":false}");
      }
        
      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
//      Serial.print("HTTP Response code: ");
//      Serial.println(httpResponseCode);

      
        
      // Free resources
      http.end();
    


}


String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
