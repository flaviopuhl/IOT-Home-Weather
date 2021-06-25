/*
 * 
 * 
   IOT_Home_weathes station 
   UI Display + Wifi manager + OTA + wifi Screen............. 1000 (from IOT_Hme_Wth_504)                                    
   IP localization + geo screen ............................. 1001  


// https://openweathermap.org/api/one-call-api
// weather codes found at https://openweathermap.org/weather-conditions
   {"cod":"200","message":0,"cnt":2,"list":[{"dt":1620756000,"main":{"temp":18.56,"feels_like":17.91,"temp_min":17.66,"temp_max":18.56,"pressure":1018,"sea_level":1018,"grnd_level":1014,"humidity":55,"temp_kf":0.9},"weather":[{"id":802,"main":"Clouds","description":"scattered clouds","icon":"03d"}],"clouds":{"all":40},"wind":{"speed":2.48,"deg":152,"gust":2.84},"visibility":10000,"pop":0.14,"sys":{"pod":"d"},"dt_txt":"2021-05-11 18:00:00"},{"dt":1620766800,"main":{"temp":17.57,"feels_like":16.9,"temp_min":15.59,"temp_max":17.57,"pressure":1018,"sea_level":1018,"grnd_level":1014,"humidity":58,"temp_kf":1.98},"weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04n"}],"clouds":{"all":60},"wind":{"speed":3.18,"deg":147,"gust":5.55},"visibility":10000,"pop":0,"sys":{"pod":"n"},"dt_txt":"2021-05-11 21:00:00"}],"city":{"id":3452925,"name":"Porto Alegre","coord":{"lat":-30.0331,"lon":-51.23},"country":"BR","population":0,"timezone":-10800,"sunrise":1620727228,"sunset":1620765732}

API call example
    https://api.openweathermap.org/data/2.5/onecall?lat=33.44&lon=-94.04&exclude=hourly,daily&appid={API key}
    https://api.openweathermap.org/data/2.5/forecast?id="3452925"&APPID="b4d2a60b9952e3dd9e52a1f1196cabe6"&mode=json&units=metric&cnt=2 HTTP/1.1
    
    https://openweathermap.org/current
    POA current weather: http://api.openweathermap.org/data/2.5/weather?id=3452925&units=metric&appid=b4d2a60b9952e3dd9e52a1f1196cabe6
                         https://api.openweathermap.org/data/2.5/onecall?lat=-30.0331&lon=-51.23&exclude=hourly,daily,minutely&units=metric&appid=b4d2a60b9952e3dd9e52a1f1196cabe6


// IP localization
https://ip-api.com/#177.51.71.181
https://www.embarcados.com.br/localizacao-aproximada-com-esp8266-parte-1/

//Wifi manager
https://github.com/alanswx/ESPAsyncWiFiManager
https://www.hackster.io/hieromon-ikasamo/esp8266-esp32-connect-wifi-made-easy-d75f45
https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/

 
//OLED
https://github.com/remoteme/esp8266-OLED >> low resource library
https://github.com/ThingPulse/esp8266-oled-ssd1306

// OTA
https://randomnerdtutorials.com/esp8266-nodemcu-ota-over-the-air-arduino/

DS18B20
https://randomnerdtutorials.com/esp8266-ds18b20-temperature-sensor-web-server-with-arduino-ide/

// NOTE: IF THIS IS NOT WORKING FOR YOU, TRY YOUR RESET BUTTON FIRST
*/

/***************************************************************************************
**                          Libraries
***************************************************************************************/

//OLED Display
#include <Wire.h>                     // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"              // legacy include: `#include "SSD1306.h"`
#include "OLEDDisplayUi.h"            // Include the UI lib
#include "images.h"                   // Include custom images

//OTA
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncTCP.h>

//Wifi manager
#include <ESPAsyncWiFiManager.h>

//Wifi
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <ESP8266HTTPClient.h>          // API access
#include <ArduinoJson.h>                // JSON decoding library

/***************************************************************************************
**                          User configurations
***************************************************************************************/

//OLED Display
SSD1306Wire  display(0x3c, D2, D1);    // Initialize the OLED display using Wire library


/***************************************************************************************
**                         Global variable declaration
***************************************************************************************/

String wifiSSID;
String wifiIP;
String wifiRSSI;

String latitude =  "-36.1111"; // 90.0000 to -90.0000 negative for Southern hemisphere
String longitude = "-56.1111"; // 180.000 to -180.000 negative for West
String currentCity;

/***************************************************************************************
**                         Pre-Setup
***************************************************************************************/

//OLED Display
OLEDDisplayUi ui     ( &display );


void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, String(millis()));
}

FrameCallback frames[] = { drawScreen1, drawScreen2, drawScreen3 }; // This array keeps function pointers to all frames
                                                                    // frames are the single views that slide in
int frameCount = 3;                                                 // how many frames are there?
OverlayCallback overlays[] = { msOverlay };                         // Overlays are statically drawn on top of a frame eg. a clock
int overlaysCount = 1;

AsyncWebServer server(80);                                          // OTA
DNSServer dns;                                                      // WifiManager

/***************************************************************************************
**                         Setup
***************************************************************************************/

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println();
  Serial.println();

      String ino = __FILE__;                                                    // Extract file name for version tracking
        ino = (ino.substring((ino.indexOf(".")), (ino.lastIndexOf("\\")) + 1));
          Serial.println(ino); 

//OLED Display
  ui.setTargetFPS(60);                                              // The ESP is capable of rendering 60fps in 80Mhz mode
                                                                    // but that won't give you much time for anything else
                                                                    // run it in 160Mhz mode or just set it to 30 fps
  ui.setActiveSymbol(activeSymbol);                                 // Customize the active and inactive symbol
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(BOTTOM);                                  // You can change this to
                                                                    // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorDirection(LEFT_RIGHT);                             // Defines where the first frame is located in the bar.
  ui.setFrameAnimation(SLIDE_LEFT);                                 // You can change the transition that is used
                                                                    // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrames(frames, frameCount);                                 // Add frames
  ui.setOverlays(overlays, overlaysCount);                          // Add overlays
  ui.init();                                                        // Initialising the UI will init the display too.
  display.flipScreenVertically();
    Serial.println("Display setup ............... Done");

    Serial.println("Starting wifi connection .... ");
  AsyncWiFiManager wifiManager(&server,&dns);                       // WifiManager
  wifiManager.autoConnect();                                        // WifiManager
    Serial.println("Done");
    
  WiFiStatus();                                                     // gets wifi status

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {     // OTA
  request->send(200, "text/plain", "IOT_Home_Weather");             // OTA
    });
    
   AsyncElegantOTA.begin(&server);                                  // Start ElegantOTA
      server.begin();   Serial.println("OTA HTTP server started");                                              

   geolocationRequest();                                            // Geolocation by IP Request      
}

/***************************************************************************************
**                         Main Loop
***************************************************************************************/

void loop() {
  int remainingTimeBudget = ui.update();
  AsyncElegantOTA.loop();                                         // OTA is verified every loop

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
  }
}

/***************************************************************************************
**                         Display Screen1
***************************************************************************************/

void drawScreen1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, "Wifi Status");

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 13, "ID..."); display->drawString(30, 13, wifiSSID);
  display->drawString(0, 26, "IP..."); display->drawString(30, 26, wifiIP);
  display->drawString(0, 39, "RSSI."); display->drawString(30, 39, wifiRSSI);
  //display->drawString(0, 52, "STS.."); display->drawString(30, 52, wl_status_to_string(WiFi.status()));

}


/***************************************************************************************
**                         Display Screen2
***************************************************************************************/

void drawScreen2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, "Geolocation");

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 13, "City:"); display->drawString(30, 13, currentCity);
  display->drawString(0, 26, "Lat :"); display->drawString(30, 26, latitude);
  display->drawString(0, 39, "Lon :"); display->drawString(30, 39, longitude);
}

/***************************************************************************************
**                         Display Screen3
***************************************************************************************/

void drawScreen3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, "Current Weather");

}
