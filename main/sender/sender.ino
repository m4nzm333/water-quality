/* ==========
   Program untuk Pengirim
=============== */
#include <Arduino.h>
// OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Lora Lib
#include <SPI.h>
#include <LoRa.h>

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Lora Pin
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 433E6

const char *deviceId = "A2";

const float ADC_RESOLUTION = 4096.0;
const int SAMPLE_NUM = 10;
// Init pH
const int PIN_PH = 36; 
// Kalibrasi saat short
const float BASE_PH = 1.53;
// Kalibrasi saat pengukuran
const float ph1x = 3.15; // Ganti sesuai voltage pada ph = 4.01
const float ph1y = 4.01;
const float ph2x = 2.33; // Ganti sesuai voltage pada ph = 1.49
const float ph2y = 9.18;
// Var util
float phVoltage;
float phSense;
// Init kekeruhan
const int PIN_TURBIDITY = 39; 
float turbidityVoltage;
int turbiditySense;

void setup()
{
  // Init pin
  pinMode(PIN_PH, INPUT);
  pinMode(PIN_TURBIDITY, INPUT);
  //initialize OLED
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  };
  display.setTextColor(WHITE);
  display.setTextSize(1);
  // Init Serial Monitor
  Serial.begin(115200);

  // Lora Setup
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

// Penghitung pH berdasarkan grafik kalibrasi pengukuran
float ph (float x) {
   float y = (((x - ph1x) * (ph2y - ph1y)) / (ph2x - ph1x)) + ph1y;
   return y;
}
void phMeasure(){
  
  float total = 0;
  int totalSample = 50;
  int baseDelay = 200;
  for(int i = 0; i < totalSample; i++){
    phVoltage = (3.3 / ADC_RESOLUTION) * analogRead(PIN_PH);
    total += phVoltage;
    delay(baseDelay);
  }
  phVoltage = total / totalSample;
  phSense = ph(phVoltage);
}

// Penghitung Kekeruhan
void turbidtyMeasure(){
  turbidityVoltage = (3.3 / ADC_RESOLUTION) * analogRead(PIN_TURBIDITY);
  // turbiditySense = (3004.74 - (280.1 * ((turbidityVoltage * turbidityVoltage) - 0.06261))) - 22.15;
  float tu1x = 0;
  float tu1y = 3000;
  float tu2x = 1.31;
  float tu2y = 0;
  float y = (((turbidityVoltage - tu1x) * (tu2y - tu1y)) / (tu2x - tu1x)) + tu1y;
  turbiditySense = int(y);
  if(turbiditySense < 0){
    turbiditySense = 0;
  }
}


int packetCounter = 0;
void loop()
{
  
  phMeasure();
  turbidtyMeasure();
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Lora Sender");
  display.setCursor(0,10);
  display.print(packetCounter);
  display.setCursor(0,20);
  display.print("----------------");
  // Tampilkan ph di led
  display.setCursor(0,30);
  display.print("pH");
  display.setCursor(0,40);
  display.print(phSense);
  display.setCursor(0,50);
  display.print(phVoltage);
  display.setCursor(30,50);
  display.print("V");
  // Tampilkan kekeruhan di led
  display.setCursor(60,30);
  display.print("Kekeruhan");
  display.setCursor(60,40);
  display.print(turbiditySense);
  display.setCursor(60,50);
  display.print(turbidityVoltage);
  display.setCursor(90,50);
  display.print("V");
  display.display();

  /* ---------------
    Lora Sender 
  --------------- */
//  LoRa.beginPacket();
//  LoRa.print(deviceId);
//  LoRa.print(";");
//  LoRa.print(phSense);
//  LoRa.endPacket();
//  delay(1000);
//  LoRa.beginPacket();
//  LoRa.print(deviceId);
//  LoRa.print(";T;");
//  LoRa.print(turbiditySense);
//  LoRa.endPacket();
//  delay(1000);

  LoRa.beginPacket();
  LoRa.print(deviceId);
  LoRa.print(";");
  LoRa.print(phSense);
  LoRa.print(";");
  LoRa.print(turbiditySense);
  LoRa.endPacket();
  delay(1000);

  
  packetCounter += 1;
  
}
