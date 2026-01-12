#include <SPI.h>
#include <Wire.h>
#include <RF24.h>
#include <RTClib.h>
#include <DHT.h>
#include <driver/rtc_io.h> 

// --- PINOUT ---
#define CE_PIN    14
#define CSN_PIN   10
#define DHT_PIN   4
#define LDR_PIN   5
#define RTC_SQW   1 
#define BTN_PIN   2 

// --- CONFIGURAÇÕES ---
#define DHT_TYPE    DHT11
#define NODE_ID     1
const uint64_t pipeAddress = 0xE8E8F0F0E1LL;

// --- OBJETOS ---
RF24 radio(CE_PIN, CSN_PIN);
RTC_DS3231 rtc;
DHT dht(DHT_PIN, DHT_TYPE);

// --- ESTRUTURAS ---
struct DataPacket {
  uint8_t nodeId;
  float temperatura;
  float umidade;
  int luminosidade;
  uint32_t timestamp;
  bool isManualRead;
};

struct ConfigPacket {
  uint32_t serverTime;      // Ignorado nesta versão, iria ser usado para atualizar o horário do RTC via dashboard
  uint32_t newIntervalMin;  // Usado para mudar intervalo
};

RTC_DATA_ATTR uint32_t sleepIntervalMinutes = 1;

DataPacket payload;
ConfigPacket receivedConfig;
bool acordouPorBotao = false;

void setup() {
  Serial.begin(115200);
  delay(500); 

  // Mantém energia no RTC
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
  
  pinMode(BTN_PIN, INPUT_PULLUP); 
  pinMode(RTC_SQW, INPUT_PULLUP);

  Wire.begin(8, 9);
  dht.begin();
  
  if (!rtc.begin()) {
    Serial.println("! ERRO: RTC !");
  } else {
    rtc.disable32K();
    rtc.writeSqwPinMode(DS3231_OFF); 
  }

  if (!radio.begin()) {
    Serial.println("! ERRO: nRF24 !");
  } else {
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_1MBPS);
    radio.setChannel(76);
    radio.enableAckPayload();
    radio.setRetries(15, 15);
    radio.openWritingPipe(pipeAddress);
  }

  verificarWakeup();
  lerSensores();
  enviarDados();
  agendarProximoAcordar();
  
  // Debounce do botão antes de dormir
  if (acordouPorBotao || digitalRead(BTN_PIN) == LOW) {
    Serial.println("Solte o botao...");
    unsigned long t = millis();
    while (digitalRead(BTN_PIN) == LOW && (millis() - t < 3000)) delay(10);
    delay(100); 
  }

  entrarEmDeepSleep();
}

void loop() {}

void verificarWakeup() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1) {
    if (wakeup_pin_mask & (1ULL << BTN_PIN)) {
      Serial.println(">>> Acordado por BOTÃO <<<");
      acordouPorBotao = true;
    } else {
      Serial.println(">>> Acordado por TIMER RTC <<<");
      acordouPorBotao = false;
    }
  } else {
    Serial.println(">>> Boot Inicial <<<");
  }
}

void lerSensores() {
  payload.nodeId = NODE_ID;
  payload.temperatura = dht.readTemperature();
  payload.umidade = dht.readHumidity();
  payload.luminosidade = analogRead(LDR_PIN);
  
  // Captura o objeto DateTime para usar no print formatado
  DateTime now = rtc.now();
  payload.timestamp = now.unixtime();
  
  payload.isManualRead = acordouPorBotao;

  if (isnan(payload.temperatura)) payload.temperatura = 0.0;
  if (isnan(payload.umidade)) payload.umidade = 0.0;
  
  // --- Print formatado ---
  Serial.printf("Leitura: T:%.1f U:%.1f L:%d TS:%u | Data: %02d/%02d/%04d %02d:%02d:%02d\n", 
                payload.temperatura, payload.umidade, payload.luminosidade, payload.timestamp,
                now.day(), now.month(), now.year(), now.hour(), now.minute(), now.second());
}

void enviarDados() {
  Serial.print("Enviando... ");
  if (radio.write(&payload, sizeof(payload))) {
    Serial.println("Sucesso (ACK).");
    if (radio.isAckPayloadAvailable()) {
      radio.read(&receivedConfig, sizeof(receivedConfig));
      if (receivedConfig.newIntervalMin > 0) {
        sleepIntervalMinutes = receivedConfig.newIntervalMin;
        Serial.printf("Novo intervalo: %d min\n", sleepIntervalMinutes);
      }
    }
  } else {
    Serial.println("Falha (Sem ACK).");
  }
  delay(10); 
  radio.powerDown(); 
}

void agendarProximoAcordar() {
  rtc.clearAlarm(1); rtc.clearAlarm(2);
  if (sleepIntervalMinutes < 1) sleepIntervalMinutes = 1;
  DateTime future = rtc.now() + TimeSpan(0, 0, sleepIntervalMinutes, 0);
  rtc.setAlarm1(future, DS3231_A1_Date);
  Serial.printf("Dormindo %d min.\n", sleepIntervalMinutes);
}

void entrarEmDeepSleep() {
  rtc_gpio_pullup_en((gpio_num_t)BTN_PIN);
  rtc_gpio_pullup_en((gpio_num_t)RTC_SQW);
  uint64_t mask = (1ULL << BTN_PIN) | (1ULL << RTC_SQW); 
  esp_sleep_enable_ext1_wakeup(mask, ESP_EXT1_WAKEUP_ANY_LOW);
  Serial.println("Zzz...");
  Serial.flush(); 
  esp_deep_sleep_start();
}