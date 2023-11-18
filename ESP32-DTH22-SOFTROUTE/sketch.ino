#include "DHTesp.h"
#include <WiFi.h>
#include <HTTPClient.h>

const int DHT_PIN = 15;

DHTesp dhtSensor;

const char* serverAddress = "http://localhost:8090/api/v1/dht22/1";
const char* authToken = "";

void setup() {
  Serial.begin(115200);
  setupWifi();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

}

void loop() {
  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
  Serial.println("Temp: " + String(data.temperature, 1) + "°C");
  Serial.println("Humidity: " + String(data.humidity, 1) + "%");
  Serial.println("---");
  
  conectToDabatase(data.temperature, data.humidity);
  delay(5000);
}

void setupWifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void conectToDabatase(float temperature, float humidity) {
   String requestBody = String("{\"temperature\":") + String(temperature, 1)  + ",\"humidity\":" + String(humidity, 1) + "}";
    Serial.println(requestBody);
    Serial.println(serverAddress);
    int httpResponseCode = sendPUTRequest(serverAddress, requestBody.c_str(), authToken);

    if (httpResponseCode == 200) {
      Serial.println("Actualizado con éxito en el servidor.");
    } else {
      Serial.print("Error en la solicitud. Código de respuesta HTTP: ");
      Serial.println(httpResponseCode);
    }
}

int sendPUTRequest(const char* serverAddress, const char* requestBody, const char* authToken) {
  HTTPClient http;
  http.begin(serverAddress);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", authToken);
  
  int httpResponseCode = http.PUT(requestBody);
  
  if (httpResponseCode == -1) {
    Serial.print("Error: ");
    Serial.println(http.errorToString(httpResponseCode).c_str());
  }

  http.end();
  return httpResponseCode;
}
