/*****************
 * Program untuk Perangkat Penerima
 * Perangkat mendeteksi pH (0 - 14) dan tingkat kekeruhan (0 - 3000 ppm) pada air, kemudian mengirim data ke perangkat penerima melalui protokol LORA
 ****************/
#include <Arduino.h>
// Library untuk OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Library yang digunakan oleh LORA
#include <SPI.h>
#include <LoRa.h>

// PIN GPIO yang digunakan oleh OLED
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// PIN GPIO yang digunakan oleh LORA
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
// Beberapa Paramter yang digunakan oleh LORA seperti Frekuensi, TX Power Antena, Bandwitdth dan lain-lain
#define BAND 915E6
#define TXPOWER 20
#define SIGNAL_BANDWIDTH 7.8E3
#define SPREADING_FACTOR 12

// Pendeteksian pH secara analog yang dibaca di perangkat adalah ADC_RESOLUTION (0 - 4096) yang kemudian dikonversi ke dalam nilai Voltase
const float ADC_RESOLUTION = 4096.0;
// Jumlah Sampel pengukuran PH sebanyak ini kemudian dirata-ratakan
const int SAMPLE_NUM = 10;
// PIN GPIO yang digunakan oleh sensor PH
const int PIN_PH = 36;
// Voltase yang diukur manual saat kalibrasi short (nilai tengah atau ph 7)
const float BASE_PH = 1.53;
// Nama Pengenalan perangkat
const char *deviceId = "A3";
// Kalibrasi pengukuran yang digunakan saat mengukur voltase manual pada tingkat ph tertentu
const float ph1x = 3.02; // Ganti sesuai voltage pada ph = 4.01
const float ph1y = 4.01;
const float ph2x = 2.23; // Ganti sesuai voltage pada ph = 1.49
const float ph2y = 9.18;
// Variabel yang digunakan untuk menyimpan hasil pengukuran ph
float phVoltage;
float phSense;

// PIN GPIO yang digunakan oleh sensor kekeruhan
const int PIN_TURBIDITY = 39;
// Variabel yang digunakan untuk menyimpan hasil pengukuran kekeruahan
float turbidityVoltage;
int turbiditySense;

void setup()
{
  // Set Mode PIN kedua sensors
  pinMode(PIN_PH, INPUT);
  pinMode(PIN_TURBIDITY, INPUT);

  // Reset Tampilan OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  };
  display.setTextColor(WHITE);
  display.setTextSize(1);

  // Init Serial Monitor
  Serial.begin(115200);

  // SETUP Semua Perangkat Lora ke Mikrokontroller
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  LoRa.setTxPower(TXPOWER);
  LoRa.setSignalBandwidth(SIGNAL_BANDWIDTH);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  if (!LoRa.begin(BAND))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}

// Fungsi untuk menghitung PH berdasarkan voltase pada pengukuran. Perhitungan digunakan dengan memanfaatkan fungsi grafik linear dilihat dari 2 pengukuran kalibrasi pada tingkat ph tertentu yang sudah disebutkan diatas
float ph(float x)
{
  float y = (((x - ph1x) * (ph2y - ph1y)) / (ph2x - ph1x)) + ph1y;
  return y;
}

// Fungsi untuk mengambil sampel ph sebanyak beberapa kali kemudian dirata-ratakan
void phMeasure()
{

  float total = 0;
  int totalSample = 25;
  int baseDelay = 200;
  for (int i = 0; i < totalSample; i++)
  {
    phVoltage = (3.3 / ADC_RESOLUTION) * analogRead(PIN_PH);
    total += phVoltage;
    delay(baseDelay);
  }
  phVoltage = total / totalSample;
  phSense = ph(phVoltage);
}

// Fungsi untuk menghitung kekeruhan dengan menggunakan fungsi linear dari voltasi saat ppm bernilai 0 hingga voltase saat ppm bernilai 3000 (berpatokan pada datasheet sensor)
void turbidtyMeasure()
{
  turbidityVoltage = (3.3 / ADC_RESOLUTION) * analogRead(PIN_TURBIDITY);
  // turbiditySense = (3004.74 - (280.1 * ((turbidityVoltage * turbidityVoltage) - 0.06261))) - 22.15;
  float tu1x = 0;
  float tu1y = 3000;
  float tu2x = 1.31;
  float tu2y = 0;
  float y = (((turbidityVoltage - tu1x) * (tu2y - tu1y)) / (tu2x - tu1x)) + tu1y;
  turbiditySense = int(y);
  if (turbiditySense < 0)
  {
    turbiditySense = 0;
  }
}

// Variabel menghitung jumlah data yang dikirim di LORA
int packetCounter = 0;

// Fungsi Loop yang dipanggil berulang-ulang
void loop()
{
  // Memanggil fungsi pengukuran
  phMeasure();
  turbidtyMeasure();

  // Menampilkan jumlah data di OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Lora Sender");
  display.setCursor(0, 10);
  display.print(packetCounter);
  display.setCursor(60, 10);
  display.print(deviceId);
  display.setCursor(0, 20);
  display.print("----------------");
  // Tampilkan ph di led
  display.setCursor(0, 30);
  display.print("pH");
  display.setCursor(0, 40);
  display.print(phSense);
  display.setCursor(0, 50);
  display.print(phVoltage);
  display.setCursor(30, 50);
  display.print("V");
  // Tampilkan kekeruhan di led
  display.setCursor(60, 30);
  display.print("Kekeruhan");
  display.setCursor(60, 40);
  display.print(turbiditySense);
  display.setCursor(60, 50);
  display.print(turbidityVoltage);
  display.setCursor(90, 50);
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

  // Mengirim paket data pengukuran melalui LORA
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
