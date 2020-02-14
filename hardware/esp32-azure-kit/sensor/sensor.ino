/* 
 * This file is part of the BlueCherry Hackathon examples.
 * Copyright (c) 2020 Daan Pape.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <WiFi.h>
#include <Wire.h>
#include <BH1750FVI.h>
#include <Sodaq_HTS221.h>
#include <PubSubClient.h>

/* IC2 bus parameters for the ESP32 Azure IoT kit */
#define I2C_SDA_PIN         25
#define I2C_SCL_PIN         26
#define IC2_SPEED           400000

/* Parameters for the WiFi connection */
#define WIFI_SSID           "DPTechnics"
#define WIFI_PASSWORD       "makkelijk"

/* Parameters for the MQTT connection */
#define MQTT_SERVER         "broker.hivemq.com"
#define MQTT_PORT           1883
#define MQTT_CLIENT_ID      "groupname"
#define MQTT_TOPIC_PREFIX   "/groupname"

/* Class instantiations */
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Sodaq_HTS221 tempHumidSensor;
BH1750FVI lightSensor(13, BH1750FVI::k_DevAddress_L, BH1750FVI::k_DevModeContHighRes);

/* Public variable for events in main loop */
int ms_counter = 0;

/**
 * Initialize the magnetometer.
 */
void magneto_init() {
  Wire.beginTransmission(0x0E);
  Wire.write(0x11);
  Wire.write(0x80);
  Wire.endTransmission();
  delay(15);
  Wire.beginTransmission(0x0E);
  Wire.write(0x10);
  Wire.write(1);
  Wire.endTransmission();
}

/**
 * Read a register from the magnetometer
 */
int magneto_read_register(int reg) {
  int reg_val;
 
  Wire.beginTransmission(0x0E);
  Wire.write(reg);
  Wire.endTransmission();
  delayMicroseconds(2);
 
  Wire.requestFrom(0x0E, 1);
  while(Wire.available()) {
    reg_val = Wire.read();
  }
 
  return reg_val;
}

/**
 * Read a value from the magnetometer.
 */
int magneto_read_value(int msb_reg, int lsb_reg) {
  int val_low, val_high;
  val_high = magneto_read_register(msb_reg);
  delayMicroseconds(2);
  val_low = magneto_read_register(lsb_reg);
  int out = (val_low|(val_high << 8));
  return out;
}

/**
 * Get the X value of the magnetometer.
 */
int magneto_get_x() {
  return magneto_read_value(0x01, 0x02);
}

/**
 * Get the Y value of the magnetometer.
 */
int magneto_get_y() {
  return magneto_read_value(0x03, 0x04);
}

/**
 * Get the Z value of the magnetometer.
 */
int magneto_get_z() {
  return magneto_read_value(0x05, 0x06);
}

/**
 * Connect to the WiFi
 */
void setup_wifi() {
  Serial.println("Connecting to WiFi");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
  }

  Serial.println("Connection success");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

/**
 * Called when an MQTT message was received.
 */
void mqtt_callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
}

/**
 * Setup the MQTT client
 */
void setup_mqtt() {
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqtt_callback);
  
  while(!mqttClient.connected()) {
    Serial.print("Connecting to MQTT server");
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("MQTT connected");
      mqttClient.publish(MQTT_TOPIC_PREFIX "/hackathon/test", MQTT_CLIENT_ID " is online");
      mqttClient.subscribe(MQTT_TOPIC_PREFIX "/hackathon/rx");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

/**
 * Setup and configure the different sensors on board
 */
void setup_sensors() {
  if(!tempHumidSensor.init()) {
    Serial.println("Could not initialize HTS221 sensor");  
  }
  tempHumidSensor.enableSensor();
  Serial.println("HTS221 sensor initialized");

  lightSensor.begin();
  Serial.println("BH1750FVI sensor initialized");

  magneto_init();
  Serial.println("MAG3110 sensor initialized");
}

/**
 * Initialize the hardware
 */
void setup() {
  /* Initialize the serial console port */
  Serial.begin(115200);
  Serial.println("BlueCherry hackathon display example");

  /* Initialize the I2C bus on the board */
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);

  setup_wifi();
  setup_mqtt();
  setup_sensors();
}

/**
 * Main application which runs in infinite loop.
 */
void loop() {
  /* Check MQTT connection */
  if(!mqttClient.connected()) {
    Serial.print("Reconnecting to MQTT server");
    if(mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("MQTT connected");
      mqttClient.subscribe(MQTT_TOPIC_PREFIX "/hackathon/rx");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

  /* Process incoming MQTT messages */
  mqttClient.loop();

  /* Execute the contents every 1000ms */
  if(ms_counter >= 1000) {
    ms_counter = 0;
    
    /* Publish temperature and humidity values */
    float temp = tempHumidSensor.readTemperature();
    float humidity = tempHumidSensor.readHumidity();
    
    char buff[64] = { 0 };
    snprintf(buff, 64, "{\"tmp\":%.02f,\"hmd\":%.02f}", temp, humidity);
    Serial.println(buff);
    mqttClient.publish(MQTT_TOPIC_PREFIX "/sensor/tempandhumid", buff);

    /* Publish lux values */
    uint16_t lux = lightSensor.GetLightIntensity();
    snprintf(buff, 64, "{\"lux\":%d}", lux);
    Serial.println(buff);
    mqttClient.publish(MQTT_TOPIC_PREFIX "/sensor/lux", buff);

    /* Publish magnetometer values */
    int x = magneto_get_x();
    int y = magneto_get_y();
    int z = magneto_get_z();
    snprintf(buff, 64, "{\"x\":%d,\"y\":%d,\"z\":%d}", x,y,z);
    Serial.println(buff);
    mqttClient.publish(MQTT_TOPIC_PREFIX "/sensor/magneto", buff);

  }
  
  ms_counter += 1;
  delay(1);
}
