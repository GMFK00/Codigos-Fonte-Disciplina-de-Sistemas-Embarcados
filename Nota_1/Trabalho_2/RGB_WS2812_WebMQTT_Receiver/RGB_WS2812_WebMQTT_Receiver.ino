#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "ESP32_WS2812_Lib.h"

// --- CONFIGURAÇÃO ---
const char* ssid = "SSID_WIFI";
const char* password = "SENHA_WIFI";
const char* mqtt_server = "IP_BROKER_MQTT"; 

// --- LED HARDWARE ---
#define LED_PIN 48 
#define LED_COUNT 1
ESP32_WS2812 strip = ESP32_WS2812(LED_COUNT, LED_PIN, 0, TYPE_GRB);

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  if (!error) {
    // 1. Extrai valores brutos
    int r = doc["r"];
    int g = doc["g"];
    int b = doc["b"];
    int bri = doc["bri"];
    
    // 2. Aplica configurações no LED
    strip.setBrightness(bri); 
    strip.setLedColorData(0, r, g, b);
    strip.show();
    
    // 3. Mostra no Serial
    Serial.printf("Comando M2M -> R:%d G:%d B:%d | Brilho:%d\n", r, g, b, bri);
    
  } else {
    Serial.println("Erro ao ler JSON");
  }
}

void setup() {
  Serial.begin(115200);
  strip.begin();
  
  // Define um brilho inicial
  strip.setBrightness(10); 
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi Conectado");
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    if (client.connect("ESP32_Atuador_LED")) {
      Serial.println("Conectado ao Broker MQTT");
      client.subscribe("laboratorio/led");
    } else {
      delay(2000);
    }
  }
  client.loop();
}