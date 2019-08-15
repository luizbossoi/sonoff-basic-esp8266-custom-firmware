#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>

String web_on_html    = "<h1>SONOFF switch is ON</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a>&nbsp;<a href=\"reset\"><button>RESET</button></a></p>";
String web_off_html   = "<h1>SONOFF switch is OFF</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a>&nbsp;<a href=\"reset\"><button>RESET</button></a></p>";
String web_reset_html = "<h1>SONOFF Network resetting...</h1>";
String web_on_json    = "{\"device\":\"" + String(ESP.getChipId()) + "\",\"status\":\"on\"}";
String web_off_json   = "{\"device\":\"" + String(ESP.getChipId()) + "\",\"status\":\"off\"}";

int DEFAULT_RELAYSTATUS = LOW;
int gpio_13_led         = 13;
int gpio_12_relay       = 12;
int gpio_0_button       = 0;


ESP8266WebServer server(80);
WiFiManager wifiManager;

void setup() {
  
  String device_name = "Sonoff-" + String(ESP.getChipId());
  
  //  Init Sonoff pins
  pinMode(gpio_13_led, OUTPUT);                       // led pin
  pinMode(gpio_12_relay, OUTPUT);                     // relay pin
  pinMode(gpio_0_button, INPUT);                      // sonoff main button input (reset network settings)
  digitalWrite(gpio_13_led, HIGH);                    // set led pin high but led off
  digitalWrite(gpio_12_relay, DEFAULT_RELAYSTATUS);   // set relay ON / OFF according to DEFAULT_RELAYSTATUS
  
  WiFi.hostname(device_name);
  wifiManager.setConfigPortalTimeout(180);
  if(!wifiManager.autoConnect(device_name.c_str())) {
    delay(3000);
    ESP.reset();
  }
  
  //if you get here you have connected to the WiFi - blink led to show its connected
  for (int i = 0; i <= 6; i++) {
    digitalWrite(gpio_13_led, HIGH);
    if(i<6) delay(200); else delay(2000);
    digitalWrite(gpio_13_led, LOW);
    delay(500);
  }
  
  digitalWrite(gpio_13_led, LOW);
  
  server.on("/", [](){
    if(digitalRead(gpio_12_relay)==HIGH) {
      server.send(200, "text/html", web_on_html);
    } else {
      server.send(200, "text/html", web_off_html);
    }
  });
  
  server.on("/on", [](){
    server.send(200, "text/html", web_on_html);
    digitalWrite(gpio_12_relay, HIGH);
  });
  
  server.on("/off", [](){
    server.send(200, "text/html", web_off_html);
    digitalWrite(gpio_12_relay, LOW);
  });

  server.on("/json", [](){
    if(digitalRead(gpio_12_relay)==HIGH) {
      server.send(200, "application/json", web_on_json);
    } else {
      server.send(200, "application/json", web_off_json);
    }
  });

  server.on("/json/on", [](){
    server.send(200, "application/json", web_on_json);
    digitalWrite(gpio_12_relay, HIGH);
  });
  
  server.on("/json/off", [](){
    server.send(200, "application/json", web_off_json);
    digitalWrite(gpio_12_relay, LOW);
  });
  

  server.on("/reset", [](){
    server.send(200, "text/html", web_reset_html);
    delay(4000);
    wifiManager.resetSettings();
    ESP.reset();
  });
  
  server.begin();
}

void loop() {
  server.handleClient();

  // control led
  if(digitalRead(gpio_12_relay)==HIGH) {
    digitalWrite(gpio_13_led, LOW);
  } else {
    digitalWrite(gpio_13_led, HIGH);
  }

  if(digitalRead(gpio_0_button)==LOW) {
    wifiManager.resetSettings();
    ESP.reset();
  }
}
