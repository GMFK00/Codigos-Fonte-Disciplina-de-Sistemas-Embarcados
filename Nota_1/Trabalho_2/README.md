# Trabalho 2: Sistema Digital Twin M2M (MQTT + WebSockets)

Este projeto implementa um sistema de **Gêmeo Digital (Digital Twin)**, onde um dispositivo físico (Controlador) e uma interface virtual (Web Dashboard) mantêm seus estados sincronizados em tempo real.

Além da sincronia visual, o projeto foca na comunicação **Machine-to-Machine (M2M)** via protocolo **MQTT**, onde o Controlador atua como um *Gateway*, traduzindo comandos do usuário para controlar um dispositivo remoto (Receptor).

## 🏗️ Arquitetura do Sistema

O sistema é composto por três nós principais operando em conjunto:

1.  **Nó Controlador (Master):** Gerencia a interface com o usuário (Botões Físicos + Display OLED) e hospeda o servidor Web. Ele atua como uma "ponte": recebe comandos via WebSocket ou Botões e publica no MQTT.
2.  **Nó Receptor (Slave):** Dispositivo passivo que apenas "escuta" (Subscribes) o tópico MQTT e replica a cor no seu LED local.
3.  **Interface Web:** Cliente HTML5/JS embarcado na memória do Controlador, comunicando-se via **WebSockets** (Porta 81) para garantir baixa latência na atualização dos sliders.

## 📡 Detalhes da Comunicação MQTT

A comunicação M2M é o coração deste projeto, permitindo o desacoplamento entre quem manda (Controlador) e quem obedece (Receptor).

*   **Broker:** É necessário um Broker MQTT (ex: Mosquitto) rodando na mesma rede.
*   **Tópico:** `laboratorio/led`
*   **Payload (JSON):**
    ```json
    {
      "r": 255,
      "g": 0,
      "b": 128,
      "bri": 100,
      "pwr": 1
    }
    ```
*   **Fluxo de Dados:**
    1.  Usuário move um slider no celular **OU** aperta um botão físico.
    2.  O **Controlador** processa a mudança e atualiza seu estado interno.
    3.  O **Controlador** publica o JSON no tópico `laboratorio/led`.
    4.  O **Receptor** recebe a mensagem instantaneamente e atualiza seu LED RGB.

## ⚙️ Hardware e Pinout

### Nó Controlador (ESP32-S3)
| Componente | GPIO | Configuração |
| :--- | :--- | :--- |
| **Botão Mode** | `15` | INPUT_PULLUP (Troca seleção: R, G, B, Brilho) |
| **Botão Down** | `16` | INPUT_PULLUP (Diminui valor) |
| **Botão Up** | `17` | INPUT_PULLUP (Aumenta valor) |
| **Botão Power** | `18` | INPUT_PULLUP (Liga/Desliga Lógico) |
| **Display SDA** | `8` | I2C Data |
| **Display SCL** | `9` | I2C Clock |

### Nó Receptor (ESP32-S3)
| Componente | GPIO | Configuração |
| :--- | :--- | :--- |
| **LED RGB** | `48` | RMT Output (Built-in WS2812) |

## 💡 Destaques da Implementação

*   **Ponte de Protocolos:** O ESP32 Controlador traduz mensagens WebSocket (vindas do browser) para MQTT (indo para o receptor) e vice-versa.
*   **Rate Limiting (Anti-Flood):** Implementação de lógica para limitar o envio de pacotes MQTT a cada `100ms`. Isso impede que o arrastar de um slider gere centenas de mensagens por segundo, o que desconectaria o dispositivo do Broker.
*   **Lógica de Botões:** Implementação de *Debounce* e aceleração (incremento rápido ao segurar o botão por mais de 400ms).
*   **Persistência de Estado (Soft-Off):** Ao desligar pelo botão "Power", o sistema envia um comando de cor preta (0,0,0) para apagar os LEDs, mas mantém a cor escolhida salva na memória RAM para quando for religado.

## 🔧 Como Executar

1.  **Broker MQTT:** Instale e rode o Mosquitto no seu PC. Descubra o IP do seu PC (ex: `ipconfig`).
2.  **Configuração do Código:**
    *   Edite `ssid` e `password` com seu Wi-Fi.
    *   Edite `mqtt_server` com o IP do seu PC.
3.  **Upload:**
    *   Carregue `RGB_WS2812_WebMQTT_Controller.ino` no primeiro ESP32.
    *   Carregue `RGB_WS2812_WebMQTT_Receiver.ino` no segundo ESP32.
4.  **Uso:** Abra o Monitor Serial do Controlador, copie o IP exibido e cole no navegador.