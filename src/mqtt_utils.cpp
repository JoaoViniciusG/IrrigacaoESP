#include "mqtt_utils.h"

BearSSL::WiFiClientSecure espClient;
PubSubClient client(espClient);
bool toConnectMqtt = false;

String topicStatus;  // esp8266/{espId}/status
String topicData;    // esp8266/{espId}/data
String topicPing;    // esp8266/{userId}/ping
String topicCommand; // esp8266/{userId}/command

String mqttUsername;
String mqttPassword;
String clientName;

static String buildDataJson();
static void onMqttMessage(char *topic, byte *payload, unsigned int length);

void reconnect()
{
  while (!client.connected() && toConnectMqtt)
  {
    Serial.print("Tentando MQTT... ");
    if (client.connect(
            clientName.c_str(),
            mqttUsername.c_str(),
            mqttPassword.c_str(),
            topicStatus.c_str(),
            1,
            true,
            "offline"))
    {
      Serial.println("✅ Conectado ao broker MQTT com sucesso!");
      client.publish(topicStatus.c_str(), "online", true);
      client.subscribe(topicPing.c_str(), 0);
      client.subscribe(topicCommand.c_str());

      String data = buildDataJson();
      mqttPublish(topicData, data);

      setStatus(Status::ENABLED);
    }
    else
    {
      disabledIfDanger();
      Serial.print("❌ Falhou, rc=");
      Serial.println(client.state());
      delay(10000);

      setStatus(Status::DISCONNECTED);
    }
  }

  if (client.connected())
  {
    client.subscribe("esp8266/+/status");
  }
}

void mqttInit(const char *server, int port, const char *userid, const char *username, const char *password, const char *name)
{
  topicStatus = "esp8266/" + String(name) + "/status";
  topicData = "esp8266/" + String(name) + "/data";
  topicPing = "esp8266/" + String(userid) + "/ping";
  topicCommand = "esp8266/" + String(name) + "/command";

  mqttUsername = strdup(username);
  mqttPassword = strdup(password);
  clientName = strdup(name);

  espClient.setInsecure();
  client.setServer(server, port);
  client.setCallback(onMqttMessage);

  Serial.print("🔧 Configurando MQTT -> ");
  Serial.print(server);
  Serial.print(":");
  Serial.println(port);
}

void mqttLoop()
{
  if (!client.connected() && toConnectMqtt)
    reconnect();
  client.loop();
}

void mqttPublish(String topic, String message)
{
  if (client.connected())
  {
    client.publish(topic.c_str(), message.c_str());
    Serial.println("📤 Mensagem enviada ao MQTT!");
  }
  else
  {
    Serial.println("⚠️ Falha ao publicar: cliente MQTT desconectado");
  }
}

static void onMqttMessage(char *topic, byte *payload, unsigned int length)
{
  String t = String(topic);
  String payloadStr;
  for (unsigned int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }

  // RESPOSTA AO PING DE DADOS //
  if (t == topicPing)
  {
    String data = buildDataJson();
    mqttPublish(topicData, data);
  }
  // EXECUTAR COMANDO //
  else if (t == topicCommand)
  {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payloadStr);

    if (error)
    {
      Serial.print("Falha ao deserializar JSON!");
      return;
    }

    if (doc["command"] == "rele")
    {
      controlRele(doc["data"]["state"], doc["data"]["time"]);
    }

    if (doc["command"] == "toggle_status")
    {
      Status currentState = getStatus();
      if (currentState == Status::ENABLED || currentState == Status::DISABLED)
      {
        Status newStatus = (currentState == Status::DISABLED) ? Status::ENABLED : Status::DISABLED;
        setStatus(newStatus);
      }
    }

    String data = buildDataJson();
    mqttPublish(topicData, data);
  }
}

static String buildDataJson()
{
  JsonDocument doc;
  doc["Id"] = config.Id;
  doc["UserId"] = config.IdUser;
  doc["IsEnabled"] = getStatus() == Status::ENABLED;
  doc["IsOpen"] = RELE_STATE;
  doc["LastMoisturePercent"] = lastMoistureReaded;

  String out;
  serializeJson(doc, out);
  return out;
}

bool getMqttStatus()
{
  return client.connected();
}