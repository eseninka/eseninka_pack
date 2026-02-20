#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

// Настройки Wi-Fi
const char* ssid = "ssid";
const char* password = "password";

// Настройки MQTT
const char* mqtt_server = "mqtt_server";  // например -> m9.wqtt.ru
const int mqtt_port = "port";             // например -> 19778
const char* mqtt_user = "name_user";      // например -> u_DZPAWC
const char* mqtt_pass = "password_user";  // например -> S6jnFzZS
const char* mqtt_topic_pub = "topic";     // например -> electro/wqtt/esp32/mes1


WiFiClient espClient;
PubSubClient client(espClient);

bool mqttConnected = false;  // Флаг подключения

void setup() {
  setupWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(256);  // Увеличиваем буфер сообщений
}

void loop() {
  if (!client.connected()) {
    mqttConnected = false;
    reconnectMQTT();
  } else {
    if (!mqttConnected) {
      mqttConnected = true;
      Serial.println("MQTT подключён стабильно!");
    }
  }
  client.loop();
  
  //Публикация сообещния
  String message = "test message";
  client.publish(mqtt_topic_pub, message.c_str()); // переводим строку в const String
}

// Подключение к WiFi
void setupWiFi() {
  Serial.println();
  Serial.print("Подключение к ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi подключён");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

//Подключение к серверу
void reconnectMQTT() {
  static unsigned long lastAttempt = 0;

  // Пытаемся переподключаться не чаще 1 раза в 5 секунд
  if (millis() - lastAttempt < 5000) {
    return;
  }
  lastAttempt = millis();

  Serial.print("Попытка подключения к MQTT...");

  // Случайный ID клиента для избежания конфликтов
  String clientId = "ESP32-" + String(random(0xFFFF), HEX);

  if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
    Serial.println("Успешно!");
    //client.subscribe(mqtt_topic_sub);
  } else {
    Serial.print("Ошибка, rc=");
    Serial.println(client.state());
  }
}