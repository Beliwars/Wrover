// ILI9341 example with embedded color bitmaps in sketch.
// WILL NOT FIT ON ARDUINO UNO OR OTHER AVR BOARDS;
// uses large bitmap image stored in array!

// Options for converting images to the format used here include:
//   http://www.rinkydinkelectronics.com/t_imageconverter565.php
// or
//  GIMP (https://www.gimp.org/) as follows:
//    1. File -> Export As
//    2. In Export Image dialog, use 'C source code (*.c)' as filetype.
//    3. Press export to get the export options dialog.
//    4. Type the desired variable name into the 'prefixed name' box.
//    5. Uncheck 'GLIB types (guint8*)'
//    6. Check 'Save as RGB565 (16-bit)'
//    7. Press export to save your image.
//  Assuming 'image_name' was typed in the 'prefixed name' box of step 4,
//  you can have to include the c file, then using the image can be done with:
//    tft.drawRGBBitmap(0, 0, image_name.pixel_data, image_name.width, image_name.height);
//  See also https://forum.pjrc.com/threads/35575-Export-for-ILI9341_t3-with-GIMP

#include "SPI.h"
#include <Adafruit_ILI9341.h>
#include "dragon.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESP32WebServer.h>

#define DC 21
#define CS 22
#define MOSI 23
#define MISO 25
#define CLK 19
#define RST 18
#define LED 5

#define BLUE 4
#define RED 0
#define GREEN 2
#define ON 1
#define OFF 0

const char* ssid = "ROBRIN01";
const char* password = "s1yexyuAzgeu7ioH69";

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(CS, DC, MOSI, CLK, RST, MISO);

ESP32WebServer server(80);

String Input = "";

void handleRoot(){
  digitalWrite (RED,ON);
  char temp[400];
  int sec = millis()/1000;
  int min = sec/60;
  int hr = min/60;

  snprintf (temp, 400,

"<html>\
  <head>\
    <title>ESP32 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP32!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
    <form method='POST' action='/input'>Escriba lo que quiera ver en el LCD:\
    <input type='text' name='input'><input type='submit' name='update' value='Update'>\
    </form>\
  </body>\
</html>",
    hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
  digitalWrite(RED,OFF);
}

void handleNotFound() {
  digitalWrite(RED,ON);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send( 404,"text/plain",message);
  digitalWrite(RED,0);
}

void setup() {
  tft.begin();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, HIGH);
  Serial.begin(115200);
  Serial.println("Dragon test");
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(":");
  }
  Serial.println("WIFI Connected to ROBRIN01");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  if(MDNS.begin("ESP32"))
    Serial.println("MDNS Responder Started");

  server.on("/",handleRoot);
  server.on("/test.svg",drawGraph);
  server.on("/input", HTTP_POST, updateText);
  server.on("/inline",[](){
    server.send (200,"text/plain","this works as well");
  });
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ("HTTP server started");
}

void updateText(){
   if(server.hasArg("input")){
    Input = server.arg("input");
   }
    digitalWrite(BLUE, !digitalRead(BLUE));              
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    tft.println("Hello World!");
    tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
    tft.println(1234.56);
    tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
    tft.println(0xDEADBEEF, HEX);
    tft.println();
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(2);
    tft.println("Lucho el basto");
    tft.println("Vamos a ver como el Lucho la vacila");
    tft.println("El propio firmemente");
    tft.println("El Wandy va a portar ArduCam ESP32");
    //for(int i=0; i < 5; i++){
    tft.fillScreen(ILI9341_WHITE);
    int w = random(0,tft.width()-DRAGON_WIDTH);
    int h = random(10,tft.height()-DRAGON_HEIGHT-10);
    tft.drawRGBBitmap(w,h,dragonBitmap,DRAGON_WIDTH, DRAGON_HEIGHT);
    tft.setCursor(w,h-10);
    tft.setTextColor(ILI9341_BLUE);
    tft.setTextSize(1);
    if(digitalRead(BLUE) == ON)
      tft.print("BLUE Led is ON");
    else
      tft.print("BLUE LED IS OFF");
    tft.setCursor(w,h+DRAGON_HEIGHT+2);
    tft.print(Input);
    Serial.println(Input);
    delay(1000);
}

void loop(void) {
   server.handleClient();
   delay(1);
}

void drawGraph(){
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x+= 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send ( 200, "image/svg+xml", out);
}

