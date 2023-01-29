#include <SPI.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI  23
#define OLED_CLK   18
#define OLED_DC    27
#define OLED_CS    5
#define OLED_RESET 17
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
int beatAvg;

void setup() {

  Serial.begin(9600);

  //Inicializace displeje
  
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();
  delay(3000);

  //Inicializace senzoru

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

}

void loop() {
  long irValue = particleSensor.getIR();

  //detecte finger
  if(irValue > 7000){ 
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE); 
    display.setCursor(50,0);                
    display.println("BPM");             
    display.setCursor(50,18);                
    display.println(beatAvg); 
    display.display();
    
    if (checkForBeat(irValue) == true) {
    
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);             
      display.setCursor(50,0);                
      display.println("BPM");             
      display.setCursor(50,18);                
      display.println(beatAvg); 
      display.display();

      //Measure duration between two beats
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;
        beatAvg = 0;

        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }

  //If no finger is detected
  } else {   
      beatAvg=0;
      display.clearDisplay();
      display.setTextSize(1);                    
      display.setTextColor(WHITE);             
      display.setCursor(30,5);                
      display.println("Please Place "); 
      display.setCursor(30,15);
      display.println("your finger ");  
      display.display();
  }     

}