/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
int buttonState = 0;
    String token="ERTY";
    const int buttonPin = 5; 
    const int ledPin =  4; 
void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();
    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP("How-To Geek", "Pa$$w0rd");

}

void loop() {
  buttonState = digitalRead(buttonPin);
//if (buttonState == HIGH) {
      // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("http://cube.konpat.me/turn.php?g=17&token="+token); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
              StaticJsonBuffer<100> jsonBuffer;
              String payload = http.getString();
              JsonObject& root = jsonBuffer.parseObject(payload);
             const char newtoken = root["token"];
            String currenttoken(newtoken);
            token =currenttoken;
            USE_SERIAL.println(token);
                USE_SERIAL.println(payload);
           }
       } else {
           USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
       }

        http.end();
      // turn LED on:
    digitalWrite(ledPin, HIGH);
  //}
   // } else {
    // turn LED off:
   // digitalWrite(ledPin, LOW);
    }else{
    USE_SERIAL.printf("cannot connect\n");}
    delay(1000);
}

