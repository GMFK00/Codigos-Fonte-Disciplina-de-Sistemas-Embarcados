#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define BTN_MODE  15 
#define BTN_UP    17
#define BTN_DOWN  16 
#define BTN_PWR   18 
#define LED_PIN_NATIVO 48 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "SSID_WIFI";
const char* password = "SENHA_WIFI";
const char* mqtt_server = "IP_BROKER_MQTT"; 

WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

unsigned long lastMqttSend = 0; 
const int INTERVALO_MQTT = 100; 
bool precisaAtualizarVisual = true;
bool precisaAtualizarRede = true;

const char* html_page = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Digital Twin Controller</title>
  <style>
    body { background-color: #121212; color: #e0e0e0; font-family: 'Segoe UI', sans-serif; text-align: center; margin: 0; padding: 20px; }
    .container { max-width: 450px; margin: auto; background: #1e1e1e; padding: 25px; border-radius: 15px; box-shadow: 0 8px 16px rgba(0,0,0,0.6); }
    
    .power-container { display: flex; justify-content: space-between; align-items: center; margin-bottom: 20px; background: #2c2c2c; padding: 10px 15px; border-radius: 10px; }
    .switch { position: relative; display: inline-block; width: 60px; height: 34px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #555; transition: .4s; border-radius: 34px; }
    .slider:before { position: absolute; content: ""; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }
    input:checked + .slider { background-color: #03dac6; }
    input:checked + .slider:before { transform: translateX(26px); }

    .preview-box { height: 80px; border-radius: 10px; margin-bottom: 20px; border: 2px solid #333; transition: background 0.2s; }
    .control-group { margin-bottom: 15px; text-align: left; }
    .label-row { display: flex; justify-content: space-between; font-size: 0.9em; margin-bottom: 5px; color: #aaa; }
    input[type=range] { width: 100%; cursor: pointer; accent-color: #bb86fc; }
    .info-panel { background: #2c2c2c; padding: 15px; border-radius: 10px; margin-top: 20px; font-family: monospace; font-size: 0.85em; text-align: left; }
    h2 { margin-top: 0; color: #bb86fc; }
    .status { font-size: 0.8em; margin-bottom: 15px; }
  </style>
</head>
<body>

<div class="container">
  <h2>Gêmeo Digital M2M</h2>
  <div class="status">WebSocket: <span id="ws-status" style="color:red">Desconectado</span></div>

  <div class="power-container">
    <span style="font-weight:bold; font-size:1.1em;">SISTEMA</span>
    <label class="switch">
      <input type="checkbox" id="pwrSwitch" onchange="uiChange()">
      <span class="slider"></span>
    </label>
  </div>

  <div id="color-preview" class="preview-box" style="background-color: rgb(0,0,0);"></div>

  <div class="control-group">
    <div class="label-row"><span>Vermelho</span><span id="txtR">0</span></div>
    <input type="range" min="0" max="255" value="0" id="sliR" oninput="uiChange()">
  </div>
  <div class="control-group">
    <div class="label-row"><span>Verde</span><span id="txtG">0</span></div>
    <input type="range" min="0" max="255" value="0" id="sliG" oninput="uiChange()">
  </div>
  <div class="control-group">
    <div class="label-row"><span>Azul</span><span id="txtB">0</span></div>
    <input type="range" min="0" max="255" value="0" id="sliB" oninput="uiChange()">
  </div>
  <div class="control-group">
    <div class="label-row"><span>Brilho</span><span id="txtBri">100</span></div>
    <input type="range" min="0" max="255" value="100" id="sliBri" oninput="uiChange()">
  </div>

  <div class="info-panel">
    <div>HEX: <span id="valHex" style="color:#03dac6">#000000</span></div>
  </div>
</div>

<script>
  var gateway = `ws://${window.location.hostname}:81/`;
  var websocket;
  var lastSend = 0;

  window.addEventListener('load', initWebSocket);

  function initWebSocket() {
    websocket = new WebSocket(gateway);
    websocket.onopen = function() { 
      document.getElementById('ws-status').innerText = 'Conectado';
      document.getElementById('ws-status').style.color = '#03dac6';
    };
    websocket.onclose = function() { 
      document.getElementById('ws-status').innerText = 'Desconectado'; 
      document.getElementById('ws-status').style.color = 'red';
      setTimeout(initWebSocket, 2000); 
    };
    websocket.onmessage = function(event) {
      var data = JSON.parse(event.data);
      var pwrState = (data.pwr === 1);
      document.getElementById('pwrSwitch').checked = pwrState;
      document.getElementById('sliR').value = data.r;
      document.getElementById('sliG').value = data.g;
      document.getElementById('sliB').value = data.b;
      document.getElementById('sliBri').value = data.bri;
      updateVisuals(data.r, data.g, data.b, data.bri, pwrState);
    };
  }

  function uiChange() {
    var now = Date.now();
    if (now - lastSend < 50) return;
    lastSend = now;

    var r = parseInt(document.getElementById('sliR').value);
    var g = parseInt(document.getElementById('sliG').value);
    var b = parseInt(document.getElementById('sliB').value);
    var bri = parseInt(document.getElementById('sliBri').value);
    var pwr = document.getElementById('pwrSwitch').checked;

    updateVisuals(r, g, b, bri, pwr);

    var msg = { r: r, g: g, b: b, bri: bri, pwr: pwr ? 1 : 0 };
    websocket.send(JSON.stringify(msg));
  }

  function updateVisuals(r, g, b, bri, pwr) {
    document.getElementById('txtR').innerText = r;
    document.getElementById('txtG').innerText = g;
    document.getElementById('txtB').innerText = b;
    document.getElementById('txtBri').innerText = bri;

    var r_vis = 0, g_vis = 0, b_vis = 0;
    if (pwr) {
      r_vis = r;
      g_vis = g;
      b_vis = b;
    }
    document.getElementById('color-preview').style.backgroundColor = `rgb(${r_vis},${g_vis},${b_vis})`;
    document.getElementById('valHex').innerText = "#" + ((1 << 24) + (r_vis << 16) + (g_vis << 8) + b_vis).toString(16).slice(1).toUpperCase();
  }
</script>
</body>
</html>
)rawliteral";

struct Estado {
  int r = 50;
  int g = 50;
  int b = 50;
  int brilho = 15;
  bool ligado = true;
  int menuIndex = 0;
};
Estado sistema;

unsigned long timerBotao = 0;
unsigned long inicioSegurar = 0;
bool segurando = false;

void setup() {
  Serial.begin(115200);
  neopixelWrite(LED_PIN_NATIVO, 0, 0, 0); 

  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_PWR, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Erro Display")); for(;;);
  }
  display.clearDisplay(); display.setTextColor(WHITE);
  display.setCursor(0,0); display.println("Conectando..."); display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  client.setServer(mqtt_server, 1883);
  server.on("/", []() { server.send(200, "text/html", html_page); });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("IP: " + WiFi.localIP().toString());
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  webSocket.loop();
  server.handleClient();

  lerBotaoPower();
  if (sistema.ligado) {
    lerBotaoMode();   
    lerBotoesAjuste();
  }

  if (precisaAtualizarVisual) {
    desenharTela();
    precisaAtualizarVisual = false;
  }

  unsigned long agora = millis();
  if (precisaAtualizarRede && (agora - lastMqttSend > INTERVALO_MQTT)) {
    atualizarRede();
    lastMqttSend = agora;
    precisaAtualizarRede = false;
  }
}

void atualizarRede() {
  char json[128];
  
  sprintf(json, "{\"r\":%d, \"g\":%d, \"b\":%d, \"bri\":%d, \"pwr\":%d}", 
          sistema.r, sistema.g, sistema.b, sistema.brilho, sistema.ligado ? 1 : 0);

  webSocket.broadcastTXT(json);

  if (client.connected()) {
    if (!sistema.ligado) {
      client.publish("laboratorio/led", "{\"r\":0,\"g\":0,\"b\":0,\"bri\":0}");
    } else {
      client.publish("laboratorio/led", json);
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      sistema.r = doc["r"];
      sistema.g = doc["g"];
      sistema.b = doc["b"];
      sistema.brilho = doc["bri"];
      int pwrState = doc["pwr"]; 
      sistema.ligado = (pwrState == 1);

      precisaAtualizarVisual = true;
      precisaAtualizarRede = true;
      
      lastMqttSend = 0; 
    }
  }
}

void lerBotaoPower() {
  if (digitalRead(BTN_PWR) == LOW) {
    delay(50);
    if (digitalRead(BTN_PWR) == LOW) {
      sistema.ligado = !sistema.ligado;
      precisaAtualizarVisual = true; 
      precisaAtualizarRede = true;
      
      lastMqttSend = 0; 
      
      while(digitalRead(BTN_PWR) == LOW);
    }
  }
}

void lerBotaoMode() {
  if (digitalRead(BTN_MODE) == LOW) {
    delay(50);
    if (digitalRead(BTN_MODE) == LOW) {
      sistema.menuIndex++; if (sistema.menuIndex > 3) sistema.menuIndex = 0;
      precisaAtualizarVisual = true;
      while(digitalRead(BTN_MODE) == LOW);
    }
  }
}

void lerBotoesAjuste() {
  bool up = (digitalRead(BTN_UP) == LOW);
  bool down = (digitalRead(BTN_DOWN) == LOW);
  if (!up && !down) { segurando = false; return; }
  unsigned long agora = millis();
  if (!segurando) {
    mudarValor(up, 1);
    inicioSegurar = agora; timerBotao = agora; segurando = true; 
    precisaAtualizarVisual = true; precisaAtualizarRede = true;
    delay(50);
  } else {
    if (agora - inicioSegurar > 400) {
      if (agora - timerBotao > 50) {
        mudarValor(up, 5); timerBotao = agora; 
        precisaAtualizarVisual = true; precisaAtualizarRede = true;
      }
    }
  }
}

void mudarValor(bool aumentar, int quantidade) {
  int* valorAlvo;
  switch(sistema.menuIndex) {
    case 0: valorAlvo = &sistema.r; break;
    case 1: valorAlvo = &sistema.g; break;
    case 2: valorAlvo = &sistema.b; break;
    case 3: valorAlvo = &sistema.brilho; break;
  }
  if (aumentar) { *valorAlvo += quantidade; if (*valorAlvo > 255) *valorAlvo = 255; } 
  else { *valorAlvo -= quantidade; if (*valorAlvo < 0) *valorAlvo = 0; }
}

void desenharTela() {
  display.clearDisplay();
  if (!sistema.ligado) {
    display.setTextSize(2); display.setCursor(45, 25); display.println("OFF"); display.display(); return;
  }
  display.setTextSize(1); display.setCursor(0,0); display.print("DIGITAL TWIN");
  if(client.connected()) display.fillCircle(124, 2, 2, WHITE);
  desenharLinha(0, "RED:", sistema.r);
  desenharLinha(1, "GRN:", sistema.g);
  desenharLinha(2, "BLU:", sistema.b);
  desenharLinha(3, "BRI:", sistema.brilho);
  display.display();
}

void desenharLinha(int id, String texto, int valor) {
  int y = 16 + (id * 12); display.setCursor(0, y);
  if (id == sistema.menuIndex) display.print("> "); else display.print("  ");
  display.print(texto);
  int w = map(valor, 0, 255, 0, 50);
  display.drawRect(60, y, 52, 8, WHITE); display.fillRect(62, y+2, w, 4, WHITE);
}

void reconnect() {
  if (client.connect("ESP32_Control_DT")) { Serial.println("MQTT OK"); }
}