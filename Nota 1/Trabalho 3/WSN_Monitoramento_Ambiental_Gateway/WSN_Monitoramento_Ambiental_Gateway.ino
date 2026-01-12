#include <SPI.h>
#include <WiFi.h>
#include <RF24.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "ESP32_WS2812_Lib.h"
#include <time.h> // Necessário para converter o Timestamp

// --- CREDENCIAIS ---
const char* ssid = "SSID_SIST_EMBARCADOS";
const char* password = "PSSWD2026";
const char* mqtt_server = "192.168.137.1"; 

// --- PINOUT ---
#define CE_PIN    14
#define CSN_PIN   10
#define LED_PIN   48 
#define LED_COUNT 1

// --- OBJETOS ---
RF24 radio(CE_PIN, CSN_PIN);
WiFiClient espClient;
PubSubClient client(espClient);
ESP32_WS2812 strip = ESP32_WS2812(LED_COUNT, LED_PIN, 0, TYPE_GRB);

const uint64_t pipeAddress = 0xE8E8F0F0E1LL;

struct DataPacket {
  uint8_t nodeId;
  float temperatura;
  float umidade;
  int luminosidade;
  uint32_t timestamp;
  bool isManualRead;
};

struct ConfigPacket {
  uint32_t serverTime;      // Não usado, iria ser usado para atualizar o horário do RTC via dashboard
  uint32_t newIntervalMin;  // Usado para mudar intervalo
};

ConfigPacket pendingConfig = {0, 0}; 

void setup() {
  Serial.begin(115200);
  strip.begin(); strip.setBrightness(20); setStatusColor(255, 0, 0);

  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);

  if (!radio.begin()) {
    Serial.println("ERRO: nRF24 nao encontrado!");
    while(1) delay(100);
  }
  
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(76);
  radio.enableAckPayload(); 
  radio.openReadingPipe(1, pipeAddress);
  radio.startListening();
  
  // Inicializa buffer zerado
  radio.writeAckPayload(1, &pendingConfig, sizeof(pendingConfig));
  
  Serial.println("Gateway Pronto.");
  setStatusColor(0, 255, 0); 
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (radio.available()) {
    DataPacket data;
    radio.read(&data, sizeof(data)); 
    
    setStatusColor(0, 0, 255); 
    
    // --- Converter Timestamp e Printar Formatado ---
    time_t rawtime = data.timestamp;
    struct tm * ti;
    ti = gmtime(&rawtime); // Converte timestamp para estrutura de tempo

    char bufferData[30];
    strftime(bufferData, 30, "%d/%m/%Y %H:%M:%S", ti);

    Serial.printf("[RADIO] ID:%d | T:%.1f C | U:%.1f %% | Lux:%d | Modo:%s | TS:%u | Data: %s\n", 
                  data.nodeId, data.temperatura, data.umidade, data.luminosidade,
                  data.isManualRead ? "MANUAL" : "AUTO", data.timestamp, bufferData);
    
    publicarDadosMQTT(data);
    
    // Recarrega o buffer para a próxima vez (apenas intervalo se houver)
    radio.writeAckPayload(1, &pendingConfig, sizeof(pendingConfig));
    if (pendingConfig.newIntervalMin > 0) pendingConfig.newIntervalMin = 0;
    
    delay(1000); 
    setStatusColor(0, 255, 0); 
  }
}

void publicarDadosMQTT(DataPacket d) {
  StaticJsonDocument<256> doc;
  doc["node"] = d.nodeId;
  doc["temp"] = d.temperatura;
  doc["umid"] = d.umidade;
  doc["lux"] = d.luminosidade;
  doc["ts"] = d.timestamp;
  doc["tipo"] = d.isManualRead ? "manual" : "auto";
  
  char buffer[256];
  serializeJson(doc, buffer);
  client.publish("fazenda/estufa/dados", buffer);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload, length);
  
  if (doc.containsKey("intervalo")) {
    pendingConfig.newIntervalMin = doc["intervalo"];
    Serial.printf("[MQTT] Novo intervalo recebido: %d min\n", pendingConfig.newIntervalMin);
    // Atualiza buffer imediatamente
    radio.writeAckPayload(1, &pendingConfig, sizeof(pendingConfig));
  }
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi conectado");
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Gateway_WSN")) {
      Serial.println("MQTT Conectado");
      client.subscribe("fazenda/estufa/config");
    } else {
      delay(5000);
    }
  }
}

void setStatusColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.setLedColorData(0, r, g, b);
  strip.show();
}