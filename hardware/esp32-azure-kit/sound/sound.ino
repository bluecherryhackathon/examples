/* 
 * This file is part of the BlueCherry Hackathon examples.
 * Copyright (c) 2020 Daan Pape, Dries Vandenbussche
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

/* The I/O port on which the buzzer is connected */
#define BUZZER_PORT   27

/* The different music tones */
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;

/**
 * Initialize the hardware
 */
void setup() {
  Serial.begin(115200);
  Serial.println("BlueCherry hackathon sound example");
  
  pinMode(BUZZER_PORT, OUTPUT);

  /* Set up PWM */
  ledcSetup(0, 5000, 8);
  ledcAttachPin(BUZZER_PORT, 0);
  ledcWrite(0, 125);
}

/**
 * Beep with the hardware PWM
 */
void beep(int note, int duration) {
  ledcWriteTone(0, note);
  ledcWrite(0, 125);
  delay(duration - 50);
  ledcWrite(0, 0);
  delay(50);
}

/**
 * Play the first section of the song.
 */
void music_first_section() {
  beep(a, 500);
  beep(a, 500);   
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150); 
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
  delay(500);

  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500); 
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
  delay(500);
}

/**
 * Play the second section of the song.
 */
void music_second_section() {
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);   
  beep(fSH, 250);
  delay(325);

  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325); 
  beep(cSH, 175); 
  beep(cH, 125); 
  beep(b, 125); 
  beep(cH, 250); 
  delay(350);
}

/**
 * Main application loop
 */
void loop() {
  music_first_section();
  music_second_section();

  //Variant 1
  beep(f, 250); 
  beep(gS, 500); 
  beep(f, 350); 
  beep(a, 125);
  beep(cH, 500);
  beep(a, 375); 
  beep(cH, 125);
  beep(eH, 650);
  delay(500);

  music_second_section();

  //Variant 2
  beep(f, 250); 
  beep(gS, 500); 
  beep(f, 375); 
  beep(cH, 125);
  beep(a, 500); 
  beep(f, 375); 
  beep(cH, 125);
  beep(a, 650); 
  delay(650);
}
