#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
#include "tinysnore.h"

Adafruit_NeoPixel strip(12, 0);

//SLEEP
long sleepTimer = 0;
long sleepInterval = 5000;
int mode = 0; //CHANGE BACK TO -1

//TIME
int curHours = 0; //CHANGE ALL BACK TO 0
int curMinutes = 0;
int curSeconds = 0;
int addHours = 0;
int addMinutes = 0;
unsigned long startTime = millis();
unsigned long endTime;
long interval = 500;
boolean change = true;
boolean clear1 = true;

//BUTTONS
boolean isButton1 = false;
boolean isButton2 = false;
int buttonPins[2] = {1, 2};
Bounce * buttons = new Bounce[2];

//FOR FUNCTIONS
int numPixel; // for breathe function
int numPixelTasks = 0; //for task tracker function
boolean reverse;
unsigned long pixelsInterval = 50;
unsigned long rainbowPreviousMillis = 0;
int rainbowCycles = 0;
#define PIXELSPIN   6
#define NUMPIXELS   12
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXELSPIN, NEO_GRB + NEO_KHZ800);

int color = 0; //0-255
int colorTimer = 0;
int colorInterval = 10;



void setup() {
  strip.begin();
  strip.clear();
  strip.setBrightness(10);
  strip.show();

  initButtons();
}


void loop() {

  updateButtons();

}


void initButtons() {
  for (int i = 0; i < 2; i++) {
    buttons[i].attach(buttonPins[i], INPUT);
    buttons[i].interval(25);
  }
}




void updateButtons() {

  for (int i = 0; i < 2; i++)  {
    buttons[i].update();
  }

  if (buttons[0].rose()) {
    isButton1 = true;
  }
  else if (buttons[0].fell()) {
    isButton1 = false;
  }
  if (buttons[1].rose()) {
    isButton2 = true;
  }
  else if (buttons[1].fell()) {
    isButton2 = false;
  }

  if (isButton1 && isButton2) {
    if (mode < 4) {
      mode++;
      isButton1 = false;
      isButton2 = false;
      clear1 = false;
      change = false;
    } else {
      mode = -1;
    }
  }
  switch (mode) {
    case -1: //Asleep
      for (int i = 0; i < 12; i++) {
        strip.setPixelColor(i, 255, 255, 255);
      }
      break;

    case 0: // Time
      setTime();
      displayTime();

      break;

    case 1: // Task Tracker
      while (change) {
        strip.clear();
        change = false;
      }
      if (isButton1) {
        strip.setPixelColor(numPixelTasks, 255, 0, 255);
        if (numPixelTasks < 12) {
          numPixelTasks++;
        }
      }
      if (isButton2) {
        strip.setPixelColor(numPixelTasks - 1, 0, 0, 0);
        if (numPixelTasks > 0) {
          numPixelTasks--;
        }
      }
      strip.show();
      break;

    case 2: // Breathe
      breathe();
      break;

    case 3: // Strand Test
      //strandTest();
      if (millis() - colorTimer > colorInterval) {
        color = (color + 1) % 255;
        colorTimer = millis();
      }

      for (int i = 0; i < 12; i++) {
        strip.setPixelColor(i, Wheel(color));
      }
      strip.show();
      break;

    case 4: //Asleep
      fallAsleep();
      break;
  }
}


void fallAsleep() {
  if (millis() - sleepTimer > sleepInterval) {
    strip.clear(); strip.show();  // Clear out NeoPixel Display
    snore(5000);                 // Go to Sleep for 5 Seconds
    sleepTimer = millis();
  }
}

void breathe() {
  while (clear1) {
    strip.clear();
    clear1 = false;
  }
  endTime = millis();
  if (reverse) {
    if (endTime - startTime >= interval) {
      strip.setPixelColor(numPixel, 0, 0, 0);
      if (numPixel > 0) {
        numPixel--;
      } else {
        reverse = false;
      }
      strip.show();
      startTime = millis();
    }
  } else {
    if (endTime - startTime >= interval) {
      strip.setPixelColor(numPixel, 38, 252, 255);
      if (numPixel < 11) {
        numPixel++;
      } else {
        reverse = true;
      }
      strip.show();
      startTime = millis();
    }
  }
}

void updateTime() {
  curSeconds = (millis() / 1) % 60;
  curMinutes = (millis() / 60) % 60;
  curHours = (millis() / 3600) % 12;
}

void setTime() {
  //updateTime();
  curSeconds = (millis() / 1) % 60;
  curMinutes = ((millis() / 60)  + addMinutes) % 60;
  curHours = ((millis() / 3600)  + addHours) % 12;

  for (int i = 0; i < 2; i++)  {
    buttons[i].update();
  }

  if (buttons[1].fell()) {
    addHours++;
  }
  if (buttons[0].fell()) {
    addMinutes += 5;
  }
}


void displayTime() {
  strip.clear();
  int minLed = map(curMinutes, 0, 60, 0, 12);

  strip.setPixelColor(curHours, 250, 155, 250); //after 1 hour has gone by, LED0 (1 o'clock) lights up
  strip.setPixelColor(minLed, 0, 251, 255);

  if (curHours == minLed) {
    strip.setPixelColor(curHours, 255, 255, 255);
  }
  strip.show();
}

void strandTest() { //no delays

  if ((unsigned long)(millis() - rainbowPreviousMillis) >= pixelsInterval) {
    rainbowPreviousMillis = millis();
    rainbow();
  }
}


void rainbow() {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, Wheel((i + rainbowCycles) & 255));
  }
  pixels.show();
  rainbowCycles++;
  if (rainbowCycles >= 256) rainbowCycles = 0;
}

/*uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}*/

void tasks() {
  while (change) {
    strip.clear();
    change = false;
  }
  for (int i = 0; i < 2; i++)  {
    buttons[i].update();
  }
  if (buttons[1].fell()) {
    strip.setPixelColor(numPixelTasks, 255, 0, 255);
    if (numPixelTasks < 12) {
      numPixelTasks++;
    }
  }
  if (buttons[0].fell()) {
    strip.setPixelColor(numPixelTasks - 1, 0, 0, 0);
    if (numPixelTasks > 0) {
      numPixelTasks--;
    }
  }
  strip.show();
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
