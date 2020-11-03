/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define TOTAL_PIXELS SCREEN_WIDTH * SCREEN_HEIGHT

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned char state1[TOTAL_PIXELS]  = { };
unsigned char state2[TOTAL_PIXELS]  = { };
unsigned char* state[] = { state1, state2 };
int currentState = 0;
int nextState = 1;

void setup() {
  Serial.begin(9600);
  randomSeed(RANDOM_REG32);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  initState();
}

void initState() {
  for(int i = 0; i < TOTAL_PIXELS; i++) {
    state[currentState][i] = random(5) == 1;
  }
}

unsigned char getCell(int x, int y) {
  if(x <= 0) {
    return 0;
  }
  if(x >= SCREEN_WIDTH) {
    return 0;
  }
  if(y <= 0) {
    return 0;
  }
  if(y >= SCREEN_HEIGHT) {
    return 0;
  }
  int pix = (y * SCREEN_WIDTH) + x;
  return state[currentState][pix];
}

int countNeightbors(int x, int y) {
  return  getCell(x - 1, y - 1) +
          getCell(x, y - 1) +
          getCell(x + 1, y - 1) +
          getCell(x + 1, y) +
          getCell(x + 1, y + 1) +
          getCell(x, y + 1) +
          getCell(x - 1, y + 1) +
          getCell(x - 1, y);
}

bool revive = false;

void update() {
  if (revive) {
    for (int i = 0; i < TOTAL_PIXELS; i++) {
      if (random(100) == 1) {
        state[currentState][i] = 1;
      }
      revive = false;
    }
  }
  int changes = 0;
  for(int i = 0; i < TOTAL_PIXELS; i++) {
    int x = i % SCREEN_WIDTH;
    int y = (int)(i / SCREEN_WIDTH);
    int currentValue = getCell(x, y);
    int neightbors = countNeightbors(x, y);
    
    if (currentValue && neightbors < 2) {
      currentValue = 0;
      changes++;
    } else if (currentValue && neightbors >= 2 && neightbors <= 3) {
      changes++;
    } else if (currentValue && neightbors > 3) {
      currentValue = 0;
      changes++;
    } else if (!currentValue && neightbors == 3) {
      currentValue = 1;
      changes++;
    }

    state[nextState][i] = currentValue;    
  }
  
  int t = nextState;
  nextState = currentState;
  currentState = t;

  if (changes < TOTAL_PIXELS * 0.05) {
    revive = true;
  }
}

void draw() {
  display.clearDisplay();
  for(int i = 0; i < TOTAL_PIXELS; i++) {
    int x = i % SCREEN_WIDTH;
    int y = (int)(i / SCREEN_WIDTH);
    display.drawPixel(x, y, state[currentState][i] ? 1 : 0);
  }
  display.display();
}

void loop() {
  draw();
  update();
}
