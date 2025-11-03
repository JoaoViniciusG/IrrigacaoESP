#include "mobile_connection.h"

unsigned long lastActivityTime = 0;
const unsigned long AP_TIMEOUT_MS = 5L * 60UL * 1000UL;

const uint16_t WS_PORT = 81;

String apiUrl = "";
String jwtToken = "";
String apSSID = "ESP_MLP_" + String(ESP.getChipId(), HEX);

String wifiSsid = "";
String wifiPass = "";

WebSocketsServer ws(WS_PORT);
bool wsClientConnected = false;
uint8_t wsClientId = 0;

void sendNetworks(uint8_t num);
bool tryConnectWifi(const String &ssid, const String &pass, String &reason);
bool postToApi(String &reason);

void awaitingMobileConnection()
{
  lastActivityTime = millis();
  WiFi.disconnect(true);

  // LIMPAR O ARQUIVO DE CONFIGURAÇÕES
  deleteConfig();

  // INICIA CONEXÃO AP
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSSID.c_str());

  Serial.println("SSID AP: " + apSSID);

  // INICIA WEBSOCKET
  ws.begin();
  ws.onEvent([](uint8_t num, WStype_t type, uint8_t *payload, size_t length)
             {
    switch (type) {
      case WStype_CONNECTED: {
        wsClientConnected = true;
        wsClientId = num;
        Serial.printf("[WS] Client %u connected\n", num);
        
        lastActivityTime = millis();

        // ENVIA REDES DISPONÍVEIS
        sendNetworks(num);
      } break;

      case WStype_TEXT: {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, payload, length);
        if (err) return;

        String t = doc["type"] | "";

        lastActivityTime = millis();

        // RETORNA AS REDES DISPONÍVEIS
        if (t == "scan") {
          sendNetworks(num);
        }
        // SALVA AS INFORMAÇÕES BASES
        else if (t == "auth_token") {
          jwtToken = doc["token"] | "";
          apiUrl = doc["url"] | "";
          Serial.printf("[WS] JWT recebido (%d bytes)\n", jwtToken.length());
        }
        // RECEBE OS DADOS DE CONEXÃO E A REALIZA
        else if (t == "provision") {
          String ssid = doc["ssid"] | "";
          String pass = doc["pass"] | "";

          // FEEDBACK: CONECTANDO AO WIFI
          JsonDocument prog; prog["type"]="provision_progress"; prog["stage"]="Conectando-se a rede Wifi...";
          String o; serializeJson(prog, o); ws.sendTXT(num, o);

          String reason;
          bool wifiOk = tryConnectWifi(ssid, pass, reason);
          if (!wifiOk) {
            JsonDocument res; res["type"]="provision_result"; res["ok"]=false; res["reason"]=reason;
            String oo; serializeJson(res, oo); ws.sendTXT(num, oo);

            sendNetworks(num);
            return;
          }

          // FEEDBACK: IP LOCAL
          prog.clear(); prog["type"]="provision_progress"; prog["stage"]="Obtendo IP local.";
          o.clear(); serializeJson(prog, o); ws.sendTXT(num, o);

          // FEEDBACK: CONECTANDO COM O SERVIDOR
          prog.clear(); prog["type"]="provision_progress"; prog["stage"]="Conectando-se aos servidores.";
          o.clear(); serializeJson(prog, o); ws.sendTXT(num, o);

          // ENVIANDO DADOS
          reason = "";
          bool apiOk = postToApi(reason);

          // FEEDBACK: FALHA NA CONEXÃO
          if (!apiOk) {
            JsonDocument res; res["type"]="provision_result"; res["ok"]=false; res["reason"]="Falha na conexão!";
            String oo; serializeJson(res, oo); ws.sendTXT(num, oo);
            return;
          }

          // RECARREGA AS CONFIGURAÇÕES DE INICIALIZAÇÃO
          loadConfig();

          // FEEDBACK: SUCESSO
          JsonDocument res; res["type"]="provision_result"; res["ok"]=true;
          String oo; serializeJson(res, oo); ws.sendTXT(num, oo);

          // RECEBE OS ÚLTIMOS PACOTES
          delay(300);

          // FECHA O WEBSOCKET E O AP
          ws.close(); 
          ws.disconnect();
          WiFi.softAPdisconnect(false);
          lastActivityTime = millis();
          setStatus(Status::ENABLED);
        }
      } break;

      // ALERTA AO CLIENTE SE DESCONECTAR
      case WStype_DISCONNECTED:
        Serial.printf("[WS] Client %u disconnected\n", num);
        if (num == wsClientId) wsClientConnected = false;
        break;
      default: break;
    } });

  Serial.println("[WS] Servidor WebSocket na porta 81 iniciado");
}

void loopAwaitingConnection()
{
  ws.loop();
  yield();

  if (lastActivityTime > 0 && millis() - lastActivityTime > AP_TIMEOUT_MS)
  {
    Serial.println("[AP] Timeout: 5 minutos sem atividade, encerrando modo AP...");
    ws.close();
    ws.disconnect();
    WiFi.softAPdisconnect(true);

    lastActivityTime = millis();
    
    setStatus(Status::STANDBY);
  }
}

void sendNetworks(uint8_t num)
{
  int n = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  JsonDocument doc;
  doc["type"] = "networks";
  JsonArray items = doc["items"].to<JsonArray>();

  for (int i = 0; i < n; i++)
  {
    JsonObject it = items.add<JsonObject>();
    it["ssid"] = WiFi.SSID(i);
    it["rssi"] = WiFi.RSSI(i);
    it["secure"] = (WiFi.encryptionType(i) != ENC_TYPE_NONE);
  }
  String out;
  serializeJson(doc, out);
  ws.sendTXT(num, out);
}

// TENTA SE CONECTAR A REDE INFORMADA
bool tryConnectWifi(const String &ssid, const String &pass, String &reason)
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  const uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000)
  {
    delay(200);
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    reason = "Falha na conexão com o Wifi!";
    return false;
  }

  wifiSsid = ssid;
  wifiPass = pass;

  Serial.printf("WiFi OK: %s  IP: %s\n", ssid.c_str(), WiFi.localIP().toString().c_str());
  return true;
}

// REALIZA O CADASTRO DO DISPOSITIVO
bool postToApi(String &reason)
{
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  String url = apiUrl + "/controlador/add";
  if (!http.begin(client, url))
  {
    reason = "http_begin";
    return false;
  }

  // HEADERS
  http.addHeader("Content-Type", "application/json");
  http.addHeader("mobile", "true");
  http.addHeader("token", jwtToken);

  // BODY
  JsonDocument doc;
  doc["identificador"] = getIdentifier();
  String body;
  serializeJson(doc, body);

  int code = http.POST(body);
  if (code != 200)
    return false;

  String payloadStr = http.getString();
  JsonDocument docResponse;

  DeserializationError err = deserializeJson(docResponse, payloadStr);
  if (err)
    return false;

  config.Id = docResponse["payload"]["Id"] | "";
  config.WifiSSID = wifiSsid;
  config.WifiPassword = wifiPass;
  config.IdUser = docResponse["payload"]["IdUser"] | "";
  config.MqttHost = docResponse["payload"]["Mqtt"]["MqttHost"] | "";
  config.MqttPort = docResponse["payload"]["Mqtt"]["MqttPort"];
  config.MqttUser = docResponse["payload"]["Mqtt"]["MqttDeviceUser"] | "";
  config.MqttPassword = docResponse["payload"]["Mqtt"]["MqttDevicePassword"] | "";
  config.BaseUrl = apiUrl;

  printConfig();
  return saveConfig();
}