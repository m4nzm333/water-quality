/* ==========
   Program untuk Penerima
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

// Wifi dan Webserver
// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WiFiAP.h>
// #include <WebServer.h>
// #include <ESPmDNS.h>
// WebServer server(80);

const char deviceId[] = "R1";
String LoRaData;

// void handleRoot() {
//   char temp[3000];

//   snprintf(temp, 3000,
//     "<html>\
//       <head>\
//         <title>Lora Deteksi Air</title>\
//         <style>\
//           thead, tbody { display: block; } \
//             tbody {\
//                 height: 500px;\
//                 overflow-y: auto;\
//                 overflow-x: hidden;\
//             }\
//         </style>\
//       </head>\
//       <body>\
//         <h1>Lora Tester</h1>\
//         <table border='2'>\
//           <thead>\
//             <th>Client</th>\
//             <th>Time</th>\
//             <th>pH</th>\
//             <th>Kekeruhan</th>\
//           </thead>\
//           <tbody id='bodyData'>\
//           </tbody>\
//         </table>\
//       </body>\
//       <script>\
//           var lastData = '';\
//           setInterval(async () => {\
//             var xmlhttp = new XMLHttpRequest();\
//             var url = 'data';\
//             xmlhttp.onreadystatechange = function(){\
//                 if (this.readyState == 4 && this.status == 200) {\
//                   var data = this.responseText.split(';');\
//                   if(this.responseText != lastData){\
//                     document.getElementById('bodyData').innerHTML =\
//                     ('<tr>\
//                       <td>'+ data[0] +'</td>\
//                       <td>2022-02-02 02:02:02</td>\
//                       <td>' + data[1] + '</td>\
//                       <td>' + data[2] + '</td>\
//                     </tr>') + document.getElementById('bodyData').innerHTML;\
//                     lastData = this.responseText;\
//                   }\
//                 };\
//             };\ 
//             xmlhttp.open('GET', url, true); \
//             xmlhttp.send(); \
//            }, 1000);\
//       </script>\
//     </html>");
//   server.send(200, "text/html", temp);
// }

// void handleNotFound() {
//   String message = "Not Found\n\n";
//   server.send(404, "text/plain", message);
// }

// void handleData() {
//   server.send(200, "text/plain", LoRaData);
// }

void setup()
{
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
  Serial.println("This is lora");

  // Lora Setup
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Wifi dan Server
  // WiFi.softAP("LoraWifi", "123456");
  // IPAddress myIP = WiFi.softAPIP();
  // Serial.print("AP IP address: ");
  // Serial.println(myIP);
  // server.begin();
  // if (MDNS.begin("esp32")) {
  //   Serial.println("MDNS responder started");
  // }
  // server.on("/", handleRoot);
  // server.on("/data", handleData);
  // server.on("/inline", []() {
  //   server.send(200, "text/plain", "this works as well");
  // });
  // server.onNotFound(handleNotFound);
  // server.begin();
}

void loop()
{
  /* ---------------
    Lora Receiver 
  --------------- */
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.print("Received packet ");
    Serial.println(packetSize);
    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");    
    Serial.println(rssi);

   // Dsiplay information
   display.clearDisplay();
   display.setCursor(0,0);
   display.print("Lora Receiver");
   display.setCursor(0,20);
   display.print("----------------");
   display.setCursor(0,30);
   display.print(LoRaData);
   display.setCursor(0,40);
   display.print("RSSI:");
   display.setCursor(30,40);
   display.print(rssi);
   display.display();   
  }

  // server.handleClient();
}
