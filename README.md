# Repositório da Disciplina de Sistemas Embarcados
## Universidade estadual do Maranhão - Sistemas Embarcados

Este repositório contém todos os trabalhos práticos desenvolvidos ao longo da disciplina de Sistemas Embarcados, com foco no desenvolvimento de projetos utilizando a placa de desenvolvimento ESP32-S3.

O objetivo é documentar o progresso, as implementações e os aprendizados obtidos em cada etapa do curso.

## Estrutura do Repositório

O repositório está organizado de acordo com as avaliações da disciplina:

---
## Nota 1 - Trabalho 1: Controle de LED WS2812 Integrado com Menu Serial
<details>
<summary>Nota 1 - Trabalho 1</summary>
Este é o primeiro projeto da disciplina, servindo como uma introdução ao controle de periféricos integrados e à interação com o usuário via comunicação serial na plataforma ESP32-S3.

### 🎯 Objetivo

O objetivo deste trabalho foi criar um sistema de controle para o LED RGB endereçável (WS2812) **integrado (built-in)** em uma placa de desenvolvimento ESP32-S3. A interação com o sistema é realizada através de um menu de opções exibido no Monitor Serial.

### ⚙️ Hardware Necessário

*   1x Placa de desenvolvimento **ESP32-S3** com LED RGB WS2812 integrado.
    *   Neste projeto, o LED da placa está conectado internamente ao **GPIO 48**.

### 📚 Software e Bibliotecas

*   **Arduino IDE** (configurada para a placa ESP32-S3)
*   **Biblioteca:** [ESP32_WS2812_Lib](https://github.com/Zhentao-Lin/ESP32_WS2812_Lib) por Zhentao-Lin.

### 💡 Funcionalidades

O firmware implementa um menu interativo que permite ao usuário:

1.  **Selecionar uma cor:** Escolher entre 7 cores pré-definidas (Vermelho, Verde, Azul, Amarelo, Magenta, Ciano e Branco).
2.  **Ajustar o brilho:** Definir um valor de brilho entre 0 (mínimo) e 255 (máximo).
3.  **Apagar o LED:** Desligar completamente o LED.

### 🚀 Como Usar

1.  **Montagem:** Nenhuma montagem externa é necessária, pois o LED WS2812 já vem integrado à placa e conectado ao GPIO 48. Basta conectar a placa ao computador com um cabo USB.
2.  **Código:** Abra o arquivo do projeto localizado em `Nota 1/Trabalho 1/BUILT_IN_ESP32-S3_RGB_WS2812/` na Arduino IDE.
3.  **Bibliotecas:** Certifique-se de ter a biblioteca `ESP32_WS2812_Lib` instalada.
4.  **Upload:** Compile e carregue o código para a sua placa ESP32-S3.
5.  **Interação:**
    *   Abra o **Monitor Serial** com a velocidade (baud rate) configurada para **115200**.
    *   O menu de opções será exibido.
    *   Digite o número da opção desejada e pressione Enter para interagir com o LED.
</details>

---

## Nota 1 - Trabalho 2: Sistema Digital Twin M2M com MQTT e WebSockets
<details>
<summary>Nota 1 - Trabalho 2</summary>
Este projeto implementa um sistema distribuído baseado no conceito de <strong>Digital Twin (Gêmeo Digital)</strong> e comunicação <strong>Machine-to-Machine (M2M)</strong>. O sistema sincroniza o estado de um controlador físico com uma interface web em tempo real e atua remotamente sobre um segundo dispositivo.

### 🎯 Objetivo

Desenvolver uma arquitetura IoT robusta que integre:
1.  **Servidor Web Embarcado:** Hospedagem de um Dashboard no próprio ESP32.
2.  **Protocolos de Comunicação:** Uso de **MQTT** (sobre TCP) para controle M2M e **WebSockets** para interface em tempo real.
3.  **Sincronia Bidirecional:** O estado do sistema deve ser idêntico tanto na interface física (Display/Botões) quanto na interface digital (Web), independente de onde ocorra a ação (física ou virtual).

### 🏗️ Arquitetura do Sistema

O projeto é dividido em três nós principais:

1.  **Nó Controlador (ESP32 #1):** Atua como o "cérebro". Possui Display OLED, botões físicos e hospeda o Servidor Web. Ele processa as entradas e envia comandos para o atuador.
2.  **Nó Receptor (ESP32 #2):** Atua como o dispositivo de campo. Recebe comandos via MQTT e controla o LED RGB (Built-in).
3.  **Broker MQTT:** Intermediário da comunicação (rodando em um PC local via Mosquitto).

### ⚙️ Hardware Necessário

*   **Controlador:**
    *   1x Placa ESP32-S3.
    *   1x Display OLED 0.96" (I2C - SSD1306).
    *   4x Push Buttons (Mode, Up, Down, Power).
*   **Receptor:**
    *   1x Placa ESP32-S3 (Utilizando o LED RGB Built-in GPIO 48).
*   **Infraestrutura:**
    *   PC rodando Broker MQTT (Eclipse Mosquitto).

### 📚 Software e Bibliotecas

*   **ArduinoJson:** Para serialização e deserialização de pacotes de dados.
*   **PubSubClient:** Para comunicação via protocolo MQTT.
*   **WebSockets** (Markus Sattler): Para comunicação full-duplex em tempo real com o navegador.
*   **Adafruit GFX & SSD1306:** Para renderização gráfica no display OLED.

### 💡 Funcionalidades Implementadas

*   **Gêmeo Digital (Digital Twin):**
    *   Ao apertar um botão físico, os sliders no Dashboard Web se movem automaticamente.
    *   Ao mover um slider no site, o display OLED e o LED remoto atualizam instantaneamente.
*   **Controle Centralizado:** Botão "Power" (Interruptor) que habilita/desabilita o sistema tanto fisicamente quanto via web.
*   **Anti-Flooding:** Implementação de *Rate Limiting* (Limitador de Taxa) para evitar congestionamento da rede MQTT/WebSocket durante ajustes rápidos.
*   **Dashboard Rico:** A interface Web calcula e exibe automaticamente a cor em formatos HEX, RGB, CMYK, HSV e HSL.

### 🚀 Como Usar

1.  **Broker:** Inicie o Mosquitto no PC (`mosquitto -c mosquitto.conf -v`) garantindo que a porta 1883 (TCP) esteja aberta.
2.  **Configuração:** Atualize as credenciais de Wi-Fi e o IP do Broker no código de ambos os ESP32.
3.  **Upload:** Carregue o código do *Controller* no ESP32 #1 e do *Receiver* no ESP32 #2.
4.  **Acesso:**
    *   Abra o Serial Monitor do Controlador para descobrir o IP atribuído.
    *   Acesse esse IP pelo navegador do celular ou PC conectado à mesma rede.
5.  **Operação:** Controle a cor e o brilho pelo site ou pelos botões físicos e observe a sincronização em tempo real entre as três pontas (Site, Display, LED).
</details>

---

## Próximos Trabalhos

A pasta para o **Trabalho 3** e as demais (`Nota2`, `Nota3`) serão preenchidas conforme os próximos projetos da disciplina forem desenvolvidos.

## Autores

*   **Gustavo de Sousa Marques**
*   **João Vítor Coelho Ferreira**
*   **João Vitor Costa Leite Virgínio da Silva**
*   **José Ribamar Cerqueira Muniz**
*   **Khalil Ravikson Alcantara do Carmo**
*   **Luís Victor Belo Ferreira**
*   **Rafael José Nogueira de Alencar**
