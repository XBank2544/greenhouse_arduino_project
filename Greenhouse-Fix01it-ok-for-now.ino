#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <ESP8266HTTPClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* WIFI_SSID = "Siriket_2.4GHz";
const char* WIFI_PASSWORD = "0014072511";
String serverName = "https://green-house-real.vercel.app/api/update";

unsigned long lastTime = 0;
unsigned long timerDelay = 50000;

int sensorPin = A0;
int limit = 500;
int relay1 = D5;
int relay2 = D6;
int relay4 = D7;
DHT dht22;

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay4, OUTPUT);
  dht22.setup(D4);
  lcd.init();
  lcd.backlight();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}
void loop() {
  delay(dht22.getMinimumSamplingPeriod());
  float humidity = dht22.getHumidity();
  float temperature = dht22.getTemperature();
  float sensorValue = analogRead(sensorPin);

  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverName + "?temperature=24.37";
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tAnalog Value");
  Serial.print(dht22.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.print(dht22.toFahrenheit(temperature), 1);
  Serial.print("\t");
  Serial.println(sensorValue, 1);

  lcd.setCursor(0, 0);
  lcd.print("Moist:     ");
  lcd.setCursor(6, 0);
  lcd.print(sensorValue);
  lcd.setCursor(12, 0);
  lcd.setCursor(16, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Temperature:     ");
  lcd.setCursor(12, 1);
  lcd.print(temperature);
  lcd.setCursor(16, 1);
  lcd.print("C");

  Serial.print("Moisture: ");
  Serial.println(sensorValue);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C ");


  if (temperature >= 36) {
    digitalWrite(relay1, LOW);
  } else if (temperature <= 33) {
    digitalWrite(relay1, HIGH);
  }

  if (temperature <= 28) {
    digitalWrite(relay2, LOW);
  } else if (temperature >= 32) {
    digitalWrite(relay2, HIGH);
  }

  if (sensorValue <= 899) {
    digitalWrite(relay4, HIGH);
  } else if (sensorValue >= 900) {
    digitalWrite(relay4, LOW);
  }


  delay(1000);
  delay(2000);
} 
