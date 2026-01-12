# 📂 Nota 1: Sistemas Embarcados

Esta pasta contém os projetos práticos desenvolvidos para a **Primeira Avaliação** da disciplina de Sistemas Embarcados. O foco desta etapa foi explorar a arquitetura do microcontrolador **ESP32-S3**, iniciando pelo controle básico de periféricos e evoluindo para arquiteturas complexas de IoT (Internet das Coisas).

## 🚀 Evolução dos Projetos

A sequência de trabalhos foi desenhada para aumentar gradativamente a complexidade técnica:

| Trabalho | Foco Principal | Tecnologias Chave |
| :--- | :--- | :--- |
| **01** | **Controle de Hardware** | GPIO, RMT (LED WS2812), Serial (UART), Máquina de Estados. |
| **02** | **Conectividade & M2M** | Wi-Fi, WebSockets, MQTT, Servidor Web, Display OLED (I2C). |
| **03** | **Redes Sem Fio & Low Power** | Rádio RF24 (SPI), Deep Sleep, RTC DS3231, Node-RED, ACK Payloads. |

---

## 📝 Resumo dos Trabalhos

### [Trabalho 1: Controle de LED Integrado](./Trabalho_1)
Um projeto introdutório focado no domínio do hardware da placa **YD-ESP32-S3**.
*   **O que faz:** Controla o LED RGB nativo (GPIO 48) através de um menu de texto via Monitor Serial.
*   **Conceito:** Abstração de hardware e interfaces de linha de comando (CLI).

### [Trabalho 2: Digital Twin M2M](./Trabalho_2)
Implementação do conceito de **Gêmeo Digital, M2M e WebServers em Sistemas Embarcados**. O estado de um dispositivo físico é espelhado em tempo real em uma interface Web.
*   **O que faz:** Um "Controlador" com display e botões comanda um "Receptor" remoto. A interface Web se move sozinha quando botões físicos são apertados.
*   **Conceito:** Comunicação Híbrida (WebSockets para interface humana + MQTT para comunicação entre máquinas).

### [Trabalho 3: WSN Monitoramento Ambiental](./Trabalho_3)
Uma **Rede de Sensores Sem Fio (WSN)** completa simulando uma estufa agrícola inteligente.
*   **O que faz:** Sensores a bateria acordam, medem temperatura/umidade, enviam via Rádio para um Gateway e voltam a dormir (Deep Sleep). O Gateway publica na nuvem para visualização no Node-RED.
*   **Conceito:** Eficiência Energética, Protocolos de Rádio (NRF24), ACK Payloads e Integração Cloud.

---

## 🛠️ Tecnologias e Ferramentas Utilizadas

### Hardware
*   **Microcontrolador:** ESP32-S3 (DevKit VCC-GND Studio).
*   **Comunicação:** Módulos nRF24L01+ (2.4GHz).
*   **Sensores/Atuadores:** DHT11, LDR, LEDs WS2812B, Display OLED SSD1306.
*   **Gerenciamento de Energia:** RTC DS3231 (Wake-up Source).

### Software
*   **IDE:** Arduino IDE (C++).
*   **Protocolos:** MQTT (TCP/IP), HTTP, WebSockets, SPI, I2C, UART.
*   **Backend/Dashboard:** Eclipse Mosquitto (Broker MQTT) e Node-RED (Fluxos visuais).

## 📦 Como Executar

Cada subpasta (`Trabalho_1`, `Trabalho_2`, `Trabalho_3`) contém seu próprio `README.md` detalhado com instruções específicas de montagem, bibliotecas necessárias e diagramas de pinagem.

Navegue até a pasta do trabalho desejado para obter mais detalhes.