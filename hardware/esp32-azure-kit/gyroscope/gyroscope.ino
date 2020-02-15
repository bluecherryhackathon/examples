/* 
 * This file is part of the BlueCherry Hackathon examples.
 * Copyright (c) 2020 Tiziano Plaiy.
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

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

/* IC2 bus parameters for the ESP32 Azure IoT kit */
#define I2C_SDA_PIN         25
#define I2C_SCL_PIN         26
#define IC2_SPEED           400000

/* Parameters for the OLED screen */
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define SCREEN_RESET        -1
#define SCREEN_ADDRESS      0x3C

/* Class instantiations */
Adafruit_SSD1306  display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET);
Adafruit_MPU6050 mpu;

/* Gyroscope data struct */
struct sensor_data {
  float acceleration_x;
  float acceleration_y;
  float acceleration_z;
  
  float gyro_x;
  float gyro_y;
  float gyro_z;

  float temperature;
};

/* dot position struct */
struct dot_position {
  int x;
  int y;
  int radius;
};

dot_position dot_pos = { 64, 32, 5 };
bool playing = true;

void initMPU() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
}

/*
 * Draw boundary lines
 */
void display_draw_boundaries() {
  // Draw horizontal lines
  display.drawFastHLine(0, 0, SCREEN_WIDTH, SSD1306_WHITE);
  display.drawFastHLine(0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SSD1306_WHITE);

  // Draw Vertical lines
  display.drawFastVLine(0, 0, SCREEN_HEIGHT, SSD1306_WHITE);
  display.drawFastVLine(SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT, SSD1306_WHITE);
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
  display_draw_boundaries();
}

/*
 * Display the first dot on the screen
 */
void draw_dot() {
    display.clearDisplay();
    display_draw_boundaries();
    display.drawCircle(dot_pos.x, dot_pos.y, dot_pos.radius, SSD1306_WHITE);
    display.display();
}

/*
 * Moves the dot according to the sensor data
 */
void move_dot(sensor_data sensorData) {
  if (!playing) {
    return;
  }

  dot_pos.x += sensorData.acceleration_x * 2;
  dot_pos.y -= sensorData.acceleration_y * 2;

  draw_dot();
  check_dot_position();
}

/*
 * Checks if the dot is still withing given boundaries
 */
void check_dot_position() {
  if (
    dot_pos.x + (dot_pos.radius / 2) >= SCREEN_WIDTH
    || dot_pos.x + (dot_pos.radius / 2) <= 0
    || dot_pos.y + (dot_pos.radius / 2) >= SCREEN_HEIGHT
    || dot_pos.y + (dot_pos.radius / 2) <= 0
    ) {
      end_game();
  }
}

/*
 * Ends the game
 */
void end_game() {
  playing = false;
  display.clearDisplay();
  display.cp437(true);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("GAME OVER!");
  display.display();
}

/*
 * Collects the sensor data in a struct
 * 
 * @returns {sensor_data} A new struct containing all the sensor data info
 */
sensor_data getSensorData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  return sensor_data {
    a.acceleration.x,
    a.acceleration.y,
    a.acceleration.z,

    g.gyro.y,
    g.gyro.x,
    g.gyro.z,

    temp.temperature
  };
};

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

  initMPU();
  setup_display();
}

/**
 * Main application which runs in infinite loop.
 */
void loop() {
  sensor_data sensorData = getSensorData();
  Serial.print("Rotation X: ");
  Serial.print(sensorData.gyro_x);
  Serial.print(", Y: ");
  Serial.print(sensorData.gyro_y);
  Serial.print(", Z: ");
  Serial.print(sensorData.gyro_z);
  Serial.println(" rad/s");
  Serial.println(playing);

  move_dot(sensorData);
  delay(1);
}
