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
2.  **Código:** Abra o arquivo do projeto localizado em `Nota1/BUILT_IN_ESP32-S3_RGB_WS2812/` na Arduino IDE.
3.  **Bibliotecas:** Certifique-se de ter a biblioteca `ESP32_WS2812_Lib` instalada.
4.  **Upload:** Compile e carregue o código para a sua placa ESP32-S3.
5.  **Interação:**
    *   Abra o **Monitor Serial** com a velocidade (baud rate) configurada para **115200**.
    *   O menu de opções será exibido.
    *   Digite o número da opção desejada e pressione Enter para interagir com o LED.
</details>

---

## Próximos Trabalhos

As pastas para o **Trabalho 2**, **Trabalho 3** e as demais (`Nota2`, `Nota3`) serão preenchidas conforme os próximos projetos da disciplina forem desenvolvidos.

## Autores

*   **Gustavo de Sousa Marques**
*   **João Vítor Coelho Ferreira**
*   **João Vitor Costa Leite Virgínio da Silva**
*   **José Ribamar Cerqueira Muniz**
*   **Khalil Ravikson Alcantara do Carmo**
*   **Luís Victor Belo Ferreira**
*   **Rafael José Nogueira de Alencar**
