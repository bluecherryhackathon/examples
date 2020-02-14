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
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* IC2 bus parameters for the ESP32 Azure IoT kit */
#define I2C_SDA_PIN         25
#define I2C_SCL_PIN         26
#define IC2_SPEED           400000

/* Parameters for the OLED screen */
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define SCREEN_RESET        -1
#define SCREEN_ADDRESS      0x3C

/* Parameters for the WiFi connection */
#define WIFI_SSID           "DPTechnics"
#define WIFI_PASSWORD       "----------"

/* Parameters for the MQTT connection */
#define MQTT_SERVER         "broker.hivemq.com"
#define MQTT_PORT           1883
#define MQTT_CLIENT_ID      "groupname"
#define MQTT_TOPIC_PREFIX   "/groupname"

/* Class instantiations */
Adafruit_SSD1306  display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/**
 * Construct the display header text line
 */
void display_set_header()
{
  display.cp437(true);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("BlueCherry Hackathon");
  display.drawLine(0,8, 128, 8, SSD1306_WHITE);
}

/**
 * Initialize the display and print program name
 */
void setup_display() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS, true, false)) {
    Serial.println("Screen initialization failed");
    while(true);
  }
  Serial.println("Display initialized");

  /* Display header */
  display.clearDisplay();
  display_set_header();
  display.display();
}

/**
 * Connect to the WiFi
 */
void setup_wifi() {
  Serial.println("Connecting to WiFi");
  display.clearDisplay();
  display_set_header();
  display.setCursor(0, 16);
  display.println("Connecting WiFi");
  display.display();

  display.clearDisplay();
  display_set_header();
  display.setCursor(0, 16);
  display.display();
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    display_set_header();
    display.setCursor(0, 16);
    display.println("Connecting WiFi.");
    display.display();
    delay(250);
    display.clearDisplay();
    display_set_header();
    display.setCursor(0, 16);
    display.println("Connecting WiFi..");
    display.display();
    delay(250);
    display.clearDisplay();
    display_set_header();
    display.setCursor(0, 16);
    display.println("Connecting WiFi...");
    display.display();
    delay(250);
    display.clearDisplay();
    display_set_header();
    display.setCursor(0, 16);
    display.println("Connecting WiFi");
    display.display();
  }

  display.clearDisplay();
  display_set_header();
  display.setCursor(0, 16);
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();
  
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

  display.clearDisplay();
  display_set_header();
  display.setCursor(0, 16);

  uint16_t xPos = 0;
  for (int i = 0; i < length; i++) {
    display.print((char) message[i]);
    display.setCursor(xPos, 16);
    xPos += 4;
  }
  display.display();
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
 * Initialize the hardware
 */
void setup() {
  /* Initialize the serial console port */
  Serial.begin(115200);
  Serial.println("BlueCherry hackathon display example");

  /* Initialize the I2C bus on the board */
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);

  setup_display();
  setup_wifi();
  setup_mqtt();
}

/**
 * Main application which runs in infinite loop.
 */
void loop() {
  /* Check MQTT connection */
  if(!mqttClient.connected()) {
    Serial.print("Reconnecting to MQTT server");
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("MQTT connected");
      mqttClient.subscribe(MQTT_TOPIC_PREFIX "/hackathon/rx");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
