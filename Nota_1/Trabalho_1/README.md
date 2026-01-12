# Trabalho 1: Controle de LED WS2812 Integrado

Este projeto marca a introdução à plataforma ESP32-S3, focando no controle de periféricos integrados (Built-in) e na interação homem-máquina via interface Serial (CLI). O objetivo foi manipular o LED RGB endereçável (WS2812) presente na placa de desenvolvimento sem a necessidade de circuitos externos.

## 📝 Descrição do Projeto

O firmware transforma o Monitor Serial da IDE em um painel de controle. Através de comandos de texto simples, o usuário pode alterar a cor do LED, ajustar a intensidade do brilho ou desligar o dispositivo. O código utiliza uma matriz de cores pré-definida para simplificar a seleção e implementa lógica bloqueante para entrada de dados numéricos (ajuste de brilho).

## ⚙️ Hardware Utilizado

*   **Microcontrolador:** ESP32-S3 (DevKit YD-ESP32-S3 ou similar).
*   **Atuador:** LED RGB WS2812B (Integrado à placa).
*   **Conexão:** Cabo USB-C (para alimentação e comunicação UART).

### Pinagem (Pinout)
| Periférico | GPIO | Função |
| :--- | :--- | :--- |
| **LED RGB** | `48` | Pino de dados RMT (Built-in) |

## 📚 Bibliotecas e Dependências

*   **[ESP32_WS2812_Lib](https://github.com/Zhentao-Lin/ESP32_WS2812_Lib):** Utilizada para controlar o protocolo de temporização precisa do LED WS2812 utilizando o periférico RMT do ESP32, evitando o bloqueio da CPU principal durante a atualização das cores.

## 🚀 Funcionalidades e Comandos

O sistema opera a `115200 baud` e aceita os seguintes comandos via Serial:

*   **`1` a `7`**: Seleciona cores pré-definidas (Vermelho, Verde, Azul, Amarelo, Magenta, Ciano, Branco).
*   **`8`**: Entra no modo de **Ajuste de Brilho**. O sistema aguarda um valor numérico (0-255) ser digitado.
*   **`9`**: Apaga o LED (Blackout), mantendo a cor selecionada na memória.

## 🔧 Como Executar

1.  Abra o arquivo `.ino` na Arduino IDE.
2.  Certifique-se de que a placa **ESP32S3 Dev Module** está selecionada.
3.  Instale a biblioteca `ESP32_WS2812_Lib`.
4.  No código, verifique se `#define LEDS_PIN 48` corresponde à sua placa.
5.  Faça o upload.
6.  Abra o Monitor Serial, configure para **115200 baud** e **New Line (Nova Linha)**.