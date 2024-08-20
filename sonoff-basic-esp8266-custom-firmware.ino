#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
#include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager
#include <DNSServer.h>
#include <ESP8266WebServer.h>

String web_on_html = "<h1>SONOFF switch is ON</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a>&nbsp;<a href=\"reset\"><button>RESET</button></a></p>";
String web_off_html = "<h1>SONOFF switch is OFF</h1><p><a href=\"on\"><button>ON</button></a>&nbsp;<a href=\"off\"><button>OFF</button></a>&nbsp;<a href=\"reset\"><button>RESET</button></a></p>";
String web_reset_html = "<h1>SONOFF Network resetting...</h1>";
String web_on_json = "{\"device\":\"" + String(ESP.getChipId()) + "\",\"status\":\"on\"}";
String web_off_json = "{\"device\":\"" + String(ESP.getChipId()) + "\",\"status\":\"off\"}";

int DEFAULT_RELAYSTATUS = LOW;
int gpio_13_led = 13;
int gpio_12_relay = 12;
int gpio_0_button = 0;
int startPressed = 0;
int timeHold = 0;

ESP8266WebServer server(80);
WiFiManager wifiManager;

void setup() {

  String device_name = "Sonoff-" + String(ESP.getChipId());

  //  Init Sonoff pins
  pinMode(gpio_13_led, OUTPUT);                      // led pin
  pinMode(gpio_12_relay, OUTPUT);                    // relay pin
  pinMode(gpio_0_button, INPUT);                     // sonoff main button input (reset network settings)
  digitalWrite(gpio_13_led, HIGH);                   // set led pin high but led off
  digitalWrite(gpio_12_relay, DEFAULT_RELAYSTATUS);  // set relay ON / OFF according to DEFAULT_RELAYSTATUS

  WiFi.hostname(device_name);
  WiFi.setAutoReconnect(true);
  wifiManager.setConfigPortalTimeout(180);
  if (!wifiManager.autoConnect(device_name.c_str())) {
    delay(3000);
    ESP.reset();
  }

  //if you get here you have connected to the WiFi - blink led to show its connected
  for (int i = 0; i <= 6; i++) {
    digitalWrite(gpio_13_led, HIGH);
    if (i < 6) delay(200);
    else delay(2000);
    digitalWrite(gpio_13_led, LOW);
    delay(500);
  }

  digitalWrite(gpio_13_led, LOW);

  server.on("/", []() {
    if (isOn()) {
      server.send(200, "text/html", web_on_html);
    } else {
      server.send(200, "text/html", web_off_html);
    }
  });

  server.on("/on", []() {
    server.send(200, "text/html", web_on_html);
    switchOn();
  });

  server.on("/off", []() {
    server.send(200, "text/html", web_off_html);
    switchOff();
  });

  server.on("/json", []() {
    if (isOn()) {
      server.send(200, "application/json", web_on_json);
    } else {
      server.send(200, "application/json", web_off_json);
    }
  });

  server.on("/json/on", []() {
    server.send(200, "application/json", web_on_json);
    switchOn();
  });

  server.on("/json/off", []() {
    server.send(200, "application/json", web_off_json);
    switchOff();
  });

  server.on("/reset", []() {
    server.send(200, "text/html", web_reset_html);
    wifiManager.resetSettings();
    delay(4000);
    ESP.reset();
  });

  server.begin();
  Serial.begin(9600);
}

void loop() {
  server.handleClient();

  // check when sonoff button is pressed to handle relay
  if (digitalRead(gpio_0_button) == LOW) {
    if (startPressed == 0) {
      startPressed = millis();
      switchRelay();
    }
    timeHold = millis() - startPressed;

    // if button is pressed more than 5 seconds, reset network
    if (timeHold >= 5000) {
      resetWifi();
    }
  } else {
    startPressed = 0;
    timeHold = 0;
  }

  keepWifiAlive();
  readSerial();
}

bool isOn() {
  return digitalRead(gpio_12_relay) == HIGH;
}

void switchRelay() {
  Serial.print("switchRelay");
  if (isOn()) {
    switchOff();
  } else {
    switchOn();
  }
}

void switchOn() {
  digitalWrite(gpio_12_relay, HIGH);
  digitalWrite(gpio_13_led, LOW);
  Serial.println("switchOn");
}

void switchOff() {
  digitalWrite(gpio_12_relay, LOW);
  digitalWrite(gpio_13_led, HIGH);
  Serial.println("switchOff");
}

void resetWifi() {
  blink();
  delay(3000);
  wifiManager.resetSettings();
  delay(3000);
  ESP.reset();
}

void blink() {
  digitalWrite(gpio_13_led, LOW);
  delay(100);
  digitalWrite(gpio_13_led, HIGH);
  delay(100);
  digitalWrite(gpio_13_led, LOW);
  delay(100);
  digitalWrite(gpio_13_led, HIGH);
  delay(100);
  digitalWrite(gpio_13_led, LOW);
  delay(100);
  digitalWrite(gpio_13_led, HIGH);
  delay(100);
  digitalWrite(gpio_13_led, LOW);
  delay(100);
  digitalWrite(gpio_13_led, HIGH);
  delay(100);
  digitalWrite(gpio_13_led, LOW);
}

wl_status_t wifiStatus;  // keep the last wifiStatus to check for changes

void keepWifiAlive() {

  if (WiFi.status() != wifiStatus) {
    Serial.print("wifiStatus changed to: ");
    Serial.println(wl_status_to_string(WiFi.status()));

    // if it was connected but is now not connected any more
    if (wifiStatus == WL_CONNECTED && WiFi.status() != WL_CONNECTED) {
      Serial.println("wifi connection lost");
      connectWifi();
    }
    wifiStatus = WiFi.status();
  }
}

void connectWifi() {
  Serial.println("connectWifi");
  wifiManager.autoConnect();
  blink();
  delay(3000);
  
  if (WiFi.status() != WL_CONNECTED) {
    // sometimes the WiFi is down and takes its time to come back. So try to reconnect in a resonable interval.
    Serial.println("WiFi is still not connected. Next try in 10s");
    delay(10000);
    connectWifi();
  }
}

void readSerial() {
  if (Serial.available() > 0) {
    String serialReceived = Serial.readStringUntil('\n');
    char commandChar = serialReceived.charAt(0);

    switch (commandChar) {
      case '1': switchOn(); break;
      case '2': switchOff(); break;
      case 'r': connectWifi(); break;
      case 'w': Serial.println(wl_status_to_string(WiFi.status())); break;
      default: Serial.println("on: 1, off: 2, reconnectWifi: r, WiFiStatus: w");
    }
  }
}

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_WRONG_PASSWORD: return "WL_WRONG_PASSWORD";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
  return "wl_status_to_string status is something unknown";
}
