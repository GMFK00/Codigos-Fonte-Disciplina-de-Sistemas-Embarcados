#include "ESP32_WS2812_Lib.h"

#define LEDS_COUNT 1
#define LEDS_PIN   48
#define CHANNEL    0

ESP32_WS2812 strip = ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

// Cores pré-definidas: {R, G, B}
uint8_t colors[7][3] = {
  {255, 0, 0},    // 1. Vermelho
  {0, 255, 0},    // 2. Verde
  {0, 0, 255},    // 3. Azul
  {255, 255, 0},  // 4. Amarelo
  {255, 0, 255},  // 5. Magenta
  {0, 255, 255},  // 6. Ciano
  {255, 255, 255}   // 7. Branco
};

int selectedColor = 0; 
int brightness = 50; 

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(brightness);
  
  atualizarLED(); 
  exibirMenu();
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    // Opções de 1 a 7 (Cores)
    if (comando == "1") { selectedColor = 0; atualizarLED(); exibirMenu(); }
    else if (comando == "2") { selectedColor = 1; atualizarLED(); exibirMenu(); }
    else if (comando == "3") { selectedColor = 2; atualizarLED(); exibirMenu(); }
    else if (comando == "4") { selectedColor = 3; atualizarLED(); exibirMenu(); }
    else if (comando == "5") { selectedColor = 4; atualizarLED(); exibirMenu(); }
    else if (comando == "6") { selectedColor = 5; atualizarLED(); exibirMenu(); }
    else if (comando == "7") { selectedColor = 6; atualizarLED(); exibirMenu(); }
    
    // Opção 8: Ajuste de Brilho
    else if (comando == "8") {
      Serial.println("\n[AJUSTE] Digite o brilho (0-255) e aperte Enter:");
      while (Serial.available() == 0) { delay(10); } // Aguarda o input
      
      int novoBrilho = Serial.parseInt();
      if (novoBrilho >= 0 && novoBrilho <= 255) {
        brightness = novoBrilho;
        strip.setBrightness(brightness); 
        atualizarLED(); // Aplica o brilho na cor atual
        Serial.print("Brilho definido em: ");
        Serial.println(brightness);
      } else {
        Serial.println("Valor inválido!");
      }
      exibirMenu();
    } 
    
    // Opção 9: Apagar
    else if (comando == "9") {
      apagarLED();
      Serial.println("\nLED Apagado!");
      exibirMenu();
    }
  }
}

void exibirMenu() {
  Serial.println("\n--- MENU DE CORES ---");
  Serial.println("Opção 1: Vermelho");
  Serial.println("Opção 2: Verde");
  Serial.println("Opção 3: Azul");
  Serial.println("Opção 4: Amarelo");
  Serial.println("Opção 5: Magenta");
  Serial.println("Opção 6: Ciano");
  Serial.println("Opção 7: Branco");
  Serial.println("Opção 8: Definir Brilho (0-255)");
  Serial.println("Opção 9: Apagar LED");
  Serial.println("---------------------");
  Serial.print("Status: Cor "); Serial.print(selectedColor + 1);
  Serial.print(" | Brilho: "); Serial.println(brightness);
}

void atualizarLED() {
  // Nesta biblioteca, o brilho só é aplicado quando enviamos os dados de cor novamente
  strip.setLedColorData(0, colors[selectedColor][0], colors[selectedColor][1], colors[selectedColor][2]);
  strip.show();
}

void apagarLED() {
  strip.setLedColorData(0, 0, 0, 0);
  strip.show();
}