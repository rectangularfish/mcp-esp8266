#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "wpa2_enterprise.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <PubSubClient.h>


WiFiClient wifiClient;


char username[64];
char password[64];
char ssid[32];

const int trigPin = 12;
const int echoPin = 14;
const char* mqtt_server = "broker.hivemq.com";

PubSubClient client(wifiClient);

#define SOUND_VELOCITY 0.034

long duration;
float distanceCm;
float distanceInch;
float previousDistance = -1;
const float threshold = 10.0;



void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("espClient123")) {
      Serial.println("connected");
      client.publish("nikolaus/jamhacks/sensor/distance", "connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }


}


void loadCredentials() {
  Serial.println("Starting loadCredentials()");

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS.");
    return;
  } else {
    Serial.println("LittleFS mounted.");
  }



  Serial.println("Opened /wifi_credentials.json");

  StaticJsonDocument<256> doc;

  File file = LittleFS.open("/wifi_credentials.json", "r");

  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return;
  }

  strncpy(username, doc["username"] | "", sizeof(username));
  strncpy(password, doc["password"] | "", sizeof(password));
  strncpy(ssid, doc["ssid"] | "", sizeof(ssid));
  file.close();

  Serial.println("Loaded credentials:");
}


void setup() {
  Serial.begin(115200);

  loadCredentials();



  //configure wifi
  wifi_set_opmode(STATION_MODE);
  struct station_config wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char *)wifi_config.ssid, ssid);
  wifi_station_set_config(&wifi_config);
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  wifi_station_set_wpa2_enterprise_auth(1);
  wifi_station_set_enterprise_identity((uint8 *)username, strlen(username));
  wifi_station_set_enterprise_username((uint8 *)username, strlen(username));
  wifi_station_set_enterprise_password((uint8 *)password, strlen(password));

  ///////// EXECUTE/////////
  wifi_station_connect();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connecting...");
    delay(500);
  }

  ///////// REPORT //////////
  Serial.println("IP address: ");  // Print wifi IP addess
  Serial.println(WiFi.localIP());






  // configure hc sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  client.setServer(mqtt_server, 1883);

}

void loop() {

  if(!client.connected()) {
    reconnect();
    }

  client.loop();
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distanceCm = duration * SOUND_VELOCITY / 2;

  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  String message = String(distanceCm);
  client.publish("nikolaus/jamhacks/sensor/distance", message.c_str());


 

}
