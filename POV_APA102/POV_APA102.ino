#include <SPI.h>
#include "control_fuente.h"
#define FASTLED_ESP8266_RAW_PIN_ORDER

#define DEBUG       0

#define DATA_PIN D7
#define CLOCK_PIN D5
#define NUM_LEDS 47
const int interruptPin = 2;
const int in0 = D0;
const int in1 = D1;
const int pwm = D2;

unsigned long t_vertical = 0;
unsigned long t_vertical_ant = 0;
unsigned long t_vertical_act = 0;
unsigned long t_total = 0;
unsigned long t_total_ant = 0;
unsigned long t_total_act = 0;


unsigned char line = 64;
unsigned char caracter;

unsigned int imageLines = 64;
uint32_t bytes[NUM_LEDS];

const char mensaje[] = {"        HOLA WORLD, THIS IS A PROOF OF WHAT THIS CAN DO... ALWAYS THINK WHAT IS NEXT?        "};
const int l_total = (sizeof(mensaje)*6)-7;
int l_ini=-64, l_pos=0;
int line2 = 0;

const long tabla[64] = {0x07BFC0, 0x1F3FF0, 0x3F3FF8, 0x7BBFF8, 0x773FF8, 0x7FBFF0, 0x7FBFE0, 0x3FF710,
                        0x1FFA20, 0x9FFC00, 0x1FDC00, 0x3FF800, 0x3FE000, 0x5FFC00, 0x3FFE00, 0x3FF900,
                        0x1FF000, 0x1FFE00, 0x1FFF00, 0x1FFD00, 0x0FF780, 0x2FF218, 0x2FE6B8, 0x1F91DC,
                        0x1F617C, 0x0E003C, 0x0E8018, 0x0F0000, 0x0C0000, 0x0C0002, 0x040004, 0x000000,
                        0x000000, 0x000800, 0x0C0000, 0x1C0000, 0x1C0000, 0x1E0000, 0x0E0000, 0x0E0000,
                        0x0FC000, 0x1FE000, 0x3FF000, 0x3FF800, 0x1FF800, 0x2FEC00, 0x0FCC00, 0x27C780,
                        0x3FD7C0, 0x3FE7C6, 0x7F87FE, 0x2003FC, 0x7003F8, 0x7003FA, 0xFC01F0, 0xFC01E0,
                        0xF801E0, 0xF80000, 0x600000, 0x680000, 0x000C00, 0x02DE00, 0x07FE00, 0x01FE00};


void send2Leds(){
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  for(int i =0; i<NUM_LEDS; i++){
    SPI.transfer((bytes[i] >> 24) & 0xFF);
    SPI.transfer((bytes[i] >> 16) & 0xFF);
    SPI.transfer((bytes[i] >> 8) & 0xFF);
    SPI.transfer(bytes[i] & 0xFF);
  }
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  SPI.transfer(0xFF);
}


void setup(){
  
  if (DEBUG){
    Serial.begin(9600);
    Serial.println("Programa DEBUG: ");
  }

  SPI.begin();
  SPI.beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE0));

  Serial.println("Begin Motor");
  pinMode(in0, OUTPUT);
  pinMode(in1, OUTPUT);
  digitalWrite(in0, HIGH);
  digitalWrite(in1, LOW);
  analogWrite(pwm, 200);
  
  Serial.println("Clear APA102");
  for(int i = 0; i < NUM_LEDS; i++) {
    bytes[i] = 0xe1000000;
  }
  send2Leds();
  delay(1000);
  
  Serial.println("Begin Interruption");
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), lapSensor, FALLING);
  Serial.println("End Set up Interruption");
   
}

void loop(){
  t_vertical_act = micros();
  if (((t_vertical_act - t_vertical_ant)>=t_vertical)&&(t_vertical!=0)){
    Imprime();
    if(DEBUG){
      //Serial.print("P: ");
      //Serial.println(line);
    }
    t_vertical_ant = t_vertical_act;
  }
}

ICACHE_RAM_ATTR void lapSensor(){
  t_total_ant = t_total_act;
  t_total_act = micros();
  t_total = t_total_act - t_total_ant;
  t_vertical = t_total/64;
  if(DEBUG){
//    Serial.print("TVC: ");
//    Serial.println(t_total);
    //Serial.print("   TP: ");
    //Serial.println(t_vertical);
  }
  
  line -= 1;
  if (line <= 0){
    line = 64; 
  }
  /*agregado*/
  l_ini += 1;
  l_pos = 0;
  if ((l_ini >=0)&&(l_ini<=l_total)){
    line2=l_ini;
  }
  if (l_ini >= (l_total+5)){
    l_ini = -64; 
  }
  /*quitado
  line2=0;
  */
}

void Imprime() {
  long mask = 0x000001;
  char caracter;
  unsigned char charpos, chardata, pl_letra, pos_letra;
  
    for(int i = 0; i < NUM_LEDS/2; i++) {
      if (tabla[line]& mask) {
        bytes[i] = 0xFF000F00;
      }else{
        bytes[i] = 0xe1F00000;
      }
      mask<<=1;
    }
    mask = 0x000001;
    for(int i = NUM_LEDS/2; i < NUM_LEDS; i++) {
      if (tabla[((line+(imageLines/2))%imageLines)]& mask) {
        bytes[i] = 0xFF000F00;
      }else{
        bytes[i] = 0xe1F00000;
      }
      mask<<=1;
    }
    
    line += 1;
    if (line >= 64){
      line = 0; 
    }
    //P2OUT = 0x00;
    l_pos += 1;
    if ((l_pos>=10)&&(l_pos<=50)){
      if ((line2 <= l_total)){
        pos_letra = line2/6;
        caracter = mensaje[pos_letra];
        pl_letra = line2%6;
        if (pl_letra < 5){
          if (caracter<0x50) {
            charpos=(((caracter&0xff)-0x20)*5)+pl_letra;
            chardata=ASCII_TABLE1[charpos];
          } else if (caracter>0x4f) {
            charpos=(((caracter&0xff)-0x50)*5)+pl_letra;
            chardata=ASCII_TABLE2[charpos];
          }
          mask = 0x000080;
          for(int i = 0; i<8; i++){
            if (chardata & mask) {
              bytes[i+31] = 0xFF0000F0;
            }
            mask>>=1;
          }
        }
        line2 += 1;
      }
    }
    send2Leds();
}
