/*****************
 * Program untuk Perangkat Penerima
 * Perangkat hanya menerima data dari Sender yang dikirim melalui protokol LORA kemudian dikirim di Serial ke Raspberry
 ****************/

// Library yang digunakan oleh LORA
#include <SPI.h>
#include <LoRa.h>

// Library untuk OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
#define SIGNAL_GAIN 6

// PIN GPIO yang digunakan oleh OLED
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;

void setup()
{
  // Init Serial Monitor dan Serial Pengiriman
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  // Reset Tampilan OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Init OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Tampilkan nama perangkat OLED
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LORA RECEIVER ");
  display.display();

  Serial.println("LoRa Receiver Test");

  // SETUP Semua Perangkat Lora ke Mikrokontroller
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  LoRa.setTxPower(TXPOWER);
  LoRa.setGain(SIGNAL_GAIN);
  LoRa.setSignalBandwidth(SIGNAL_BANDWIDTH);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);

  if (!LoRa.begin(BAND))
  {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0, 10);
  display.println("LoRa Initializing OK!");
  display.display();
}

void loop()
{

  // Mencoba untuk konversi paket signal pengirim ke bentuk yang bisa dibaca
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // Menerima paket data
    Serial.print("Received packet ");

    // Membaca Data
    while (LoRa.available())
    {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    // Tampilkan RSSI (Kekuatan Signal Pengirim)
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");
    Serial.println(rssi);

    // Memunculkan isi data ke OLED Penerima
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("LORA RECEIVER");
    display.setCursor(0, 20);
    display.print("Received packet:");
    display.setCursor(0, 30);
    display.print(LoRaData);
    display.setCursor(0, 40);
    display.print("RSSI:");
    display.setCursor(30, 40);
    display.print(rssi);
    display.display();
    Serial2.print(LoRaData);
  }
}
