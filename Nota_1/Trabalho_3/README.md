# Trabalho 3: WSN de Monitoramento Ambiental Low-Power

Este projeto finaliza a primeira nota com a implementação de uma **Rede de Sensores Sem Fio (WSN)** focada em eficiência energética e monitoramento remoto. O sistema utiliza transceptores de rádio frequência (RF 2.4GHz) para comunicação entre nós e integra-se à nuvem via MQTT e Node-RED.

## 🎯 Objetivo

Desenvolver uma arquitetura completa de IoT composta por:
1.  **Nó Sensor (End Device):** Dispositivo alimentado a bateria que opera em ciclos de *Deep Sleep*, acordando apenas para ler sensores, transmitir dados e voltar a dormir.
2.  **Gateway:** Dispositivo central (Always On) que recebe os dados via Rádio (RF24), conecta-se ao Wi-Fi e publica as informações em um Broker MQTT.
3.  **Dashboard:** Interface gráfica no Node-RED para visualização de dados históricos e envio de configurações remotas (alteração do intervalo de leitura).

## 🏗️ Arquitetura e Fluxo de Dados

1.  **Sensor:** Acorda (RTC) ➔ Lê (DHT11/LDR) ➔ Envia (RF24) ➔ Recebe ACK com Config ➔ Dorme.
2.  **Gateway:** Recebe (RF24) ➔ Indica Status (LED) ➔ Converte JSON ➔ Publica (MQTT).
3.  **Node-RED:** Assina Tópico ➔ Dashboard ➔ Usuário define Intervalo ➔ Publica Config.

## ⚙️ Hardware e Pinout

### 1. Nó Sensor (Low Power)
O sensor é projetado para operar desconectado, utilizando o RTC DS3231 para acordar o ESP32 através de interrupção externa.

| Componente | GPIO | Função | Observação |
| :--- | :--- | :--- | :--- |
| **NRF24 CE** | `14` | Chip Enable | Controle de TX/RX |
| **NRF24 CSN** | `10` | Chip Select | Seleção SPI |
| **DHT11** | `4` | Dados | Temp/Umid |
| **LDR** | `5` | Analog In | Luminosidade |
| **RTC SQW** | `1` | **Wakeup Source** | Pino de alarme do DS3231 |
| **Botão** | `2` | **Wakeup Source** | Leitura Manual |
| **I2C SDA** | `8` | Data | Comunicação RTC |
| **I2C SCL** | `9` | Clock | Comunicação RTC |

### 2. Gateway (Always On)
Dispositivo ligado na tomada, atuando como ponte RF <-> WiFi.

| Componente | GPIO | Função |
| :--- | :--- | :--- |
| **NRF24 CE** | `14` | Chip Enable |
| **NRF24 CSN** | `10` | Chip Select |
| **LED Status** | `48` | Indicação Visual (WS2812) |

## 🚦 Indicadores de Status (Gateway)

O Gateway possui um LED RGB (GPIO 48) que fornece feedback visual sobre o estado da operação:

1.  🔴 **Vermelho:** Inicialização do sistema, conexão Wi-Fi/MQTT em andamento ou falha no hardware do Rádio.
2.  🟢 **Verde:** Sistema Online, conectado ao Broker MQTT e aguardando pacotes dos sensores (Estado Ocioso).
3.  🔵 **Azul:** Atividade de RF detectada (Recebendo pacote de dados do sensor e processando envio MQTT).

## 📊 Node-RED Dashboard

O arquivo `flows.json` incluído configura um dashboard completo contendo:
*   **Gauges:** Visualização instantânea de Temperatura e Umidade.
*   **Gráficos (Charts):** Histórico de evolução da luminosidade e clima.
*   **Indicadores de Texto:** Data da última leitura e Modo de operação (Manual/Auto).
*   **Controle Bidirecional:** Um **Slider** permite alterar o intervalo de sono do sensor (de 1 a 60 minutos).

> **Nota:** O dashboard está configurado no caminho `/dashboard`.

## 🛠️ Tecnologias Chave

*   **Deep Sleep & RTC DS3231:** O ESP32 desliga quase todos os periféricos e confia no RTC externo para gerar o pulso de acordar (Wakeup), maximizando a bateria.
*   **ACK Payloads (NRF24):** Técnica onde o Gateway envia dados de configuração (Downlink) "de carona" no pacote de confirmação de recebimento, eliminando a necessidade do sensor ficar acordado ouvindo o rádio.

## 🔧 Instalação e Execução

1.  **Bibliotecas Necessárias:**
    *   `RF24`, `PubSubClient`, `ArduinoJson`, `RTClib`, `DHT sensor library`, `ESP32_WS2812_Lib`.
2.  **Configuração do Gateway:**
    *   No arquivo `WSN_Monitoramento_Ambiental_Gateway.ino`, configure seu Wi-Fi e IP do Broker MQTT.
3.  **Configuração do Sensor:**
    *   No arquivo `WSN_Monitoramento_Ambiental_No_Sensor.ino`, verifique se `pipeAddress` é o mesmo do Gateway.
4.  **Node-RED:**
    *   Importe o `flows.json`.
    *   Configure o nó `mqtt-broker` para apontar para o seu servidor Mosquitto.
    *   Acesse `http://localhost:1880/dashboard`.