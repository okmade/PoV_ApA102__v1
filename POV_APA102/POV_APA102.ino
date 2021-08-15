#include <SPI.h>
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#define DATA_PIN D7
#define CLOCK_PIN D5
#define NUM_LEDS 46
const int interruptPin = 2;
const int in0 = D0;
const int in1 = D1;
const int pwm = D2;

unsigned long currentTimeLap = 0;
unsigned long oldTimeLap = 0;
unsigned long currentTime = 0;
unsigned long oldTime = 0;
unsigned long timeDiv;
unsigned long deltaLap;
unsigned long deltaTime;
unsigned int posLine = 0;
unsigned int imageLines = 94;

byte r =255;
byte g = 0;
byte b = 0;

CRGB leds[NUM_LEDS];

ICACHE_RAM_ATTR void lapSensor(){
  currentTimeLap = micros();
  deltaLap = currentTimeLap - oldTimeLap;
  oldTimeLap = currentTimeLap;
  timeDiv = deltaLap/imageLines;
  posLine = 0;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Begin strip");
  FastLED.addLeds<APA102,DATA_PIN,CLOCK_PIN,BGR,DATA_RATE_MHZ(20)>(leds,NUM_LEDS);
  FastLED.setBrightness(255);
  FastLED.clear();
  FastLED.show();
  
  Serial.println("Begin Motor");
  pinMode(in0, OUTPUT);
  pinMode(in1, OUTPUT);
  digitalWrite(in0, HIGH);
  digitalWrite(in1, LOW);
  analogWrite(pwm, 175);
  
  Serial.println("Begin Interruption");
  oldTimeLap = micros();
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), lapSensor, FALLING);
  Serial.println("End Set up Interruption");
}

void loop() {
  currentTime = micros();
  deltaTime = (currentTime - oldTime);
  if (deltaTime >= timeDiv){
    oldTime = currentTime;
    run2();
  }
}

void run1(){
  FastLED.clear();
  if (posLine < (imageLines/4)){
    for (int i = 0; i < NUM_LEDS; i++){
      if (i < NUM_LEDS/2){
        leds[i] = CRGB::Red;
      }else{
        leds[i] = CRGB::Yellow;
      }
    }
  }else if (posLine < ((imageLines/4)*2)){
    for (int i = 0; i < NUM_LEDS; i++){
      if (i < NUM_LEDS/2){
        leds[i] = CRGB::Blue;
      }else{
        leds[i] = CRGB::Green;
      }
    }
  }else if (posLine < ((imageLines/4)*3)){
    for (int i = 0; i < NUM_LEDS; i++){
      if (i < NUM_LEDS/2){
        leds[i] = CRGB::Yellow;
      }else{
        leds[i] = CRGB::Red;
      }
    }
  }else {
    for (int i = 0; i < NUM_LEDS; i++){
      if (i < NUM_LEDS/2){
        leds[i] = CRGB::Green;
      }else{
        leds[i] = CRGB::Blue;
      }
    }
  }
  FastLED.show();
  posLine ++;
  if (posLine >=imageLines){
    posLine = 0;
  }
}

void run2(){
  FastLED.clear();
  if (posLine < 23){
    leds[posLine] = CRGB::Blue;
  }else{
    leds[posLine] = CRGB::Green;
  }
  FastLED.show();
  posLine ++;
  if (posLine >=47){
    posLine = 0;
  }
}
