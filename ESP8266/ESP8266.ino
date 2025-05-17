#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "wpa2_enterprise.h"
#include <LittleFS.h>
#include <ArduinoJson.h>


char username[64];
char password[64];
char ssid[32];

const int trigPin = 12;
const int echoPin = 14;

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;



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
  ///////// CONFIGURE //////////
  wifi_set_opmode(STATION_MODE);
  struct station_config wifi_config; 
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char *)wifi_config.ssid, ssid);
  wifi_station_set_config(&wifi_config);
  // DISABLE authentication using certificates - But risk leaking your password to someone claiming to be "eduroam"
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  // Authenticate using username/password
  wifi_station_set_wpa2_enterprise_auth(1); 
  wifi_station_set_enterprise_identity((uint8 *)username, strlen(username));
  wifi_station_set_enterprise_username((uint8 *)username, strlen(username));
  wifi_station_set_enterprise_password((uint8 *)password, strlen(password));

  ///////// EXECUTE/////////
  wifi_station_connect();
  while (WiFi.status() != WL_CONNECTED) {Serial.println("Wifi connecting..."); delay(500);}

  ///////// REPORT //////////
  Serial.println("IP address: ");  // Print wifi IP addess
  Serial.println(WiFi.localIP());






  // configure hc sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  
  distanceCm = duration * SOUND_VELOCITY/2;
  
  distanceInch = distanceCm * CM_TO_INCH;
  
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  
  delay(1000);
}
