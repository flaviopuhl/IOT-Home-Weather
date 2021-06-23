/* 
 *  Sketch for ESP32 to fetch the Weather Forecast from OpenWeather
 *  an example from the library here: https://github.com/Bodmer/OpenWeather
 *  
 *  Sign up for a key and read API configuration info here: 
 *  https://openweathermap.org/

   IOT_Home_weathes station
   Weather data download ............. 100  ok                                     
   IP localization ................... 203  ok
   OTA ............................... 301  ok    http://"ip" + "/update"
   Wifi Manager ...................... 401  ok
   DS18B20 ........................... 500
   OLED .............................. 600
   
   
   

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
#include <Arduino.h>
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
#else // ESP32
  #include <WiFi.h>
#endif
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>  // http web access library
#include <JSON_Decoder.h>       
#include <ArduinoJson.h>        // JSON decoding library
#include <OpenWeather.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Time.h>               // Just using this library for unix time conversion
#include <TimeLib.h>

//#include <SPI.h>              // OLED SSD1306SimpleDemo
#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
//#include "OLED.h"
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
#include "images.h"             // Optionally include custom images
#include "OLEDDisplayUi.h"      // Include the UI lib

#include <ESPAsyncWiFiManager.h>

#include <stdlib.h>
#include "IOT_Credentials.h"
#include "IOT_Declarations.h"

#include <ESPAsyncTCP.h>         //OTA
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <OneWire.h>             // DS18B20
#include <DallasTemperature.h>


/***************************************************************************************
**                          User configurations
***************************************************************************************/

//OLED display(4, 5);
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h
OLEDDisplayUi ui     ( &display );     // Display user interface feature

// GPIO where the DS18B20 is connected to
const int oneWireBus = 15;  //ESP8266 D2 (GPIO 4)



/***************************************************************************************
**                         Code starts here
***************************************************************************************/

OW_Weather ow;                // Weather forecast library instance

AsyncWebServer server(80);    // OTA
DNSServer dns;                // WifiManager

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {   //OLED ui
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, String(millis()));
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, updateScreensWifi, updateScreensGeo};

// how many frames are there?
int frameCount = 5;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

/***************************************************************************************
**                         Setup
***************************************************************************************/
void setup() {
   
  Serial.begin(115200);       // Fast to stop it holding up the stream
    delay(4000);
 
  sensors.begin();            // Start the DS18B20 sensor

  display.init(); // Initialising the UI will init the display too.
  display.flipScreenVertically(); 
  display.clear(); // clear the display
         
      String ino = __FILE__;                                                    // Extract file name for version tracking
        ino = (ino.substring((ino.indexOf(".")), (ino.lastIndexOf("\\")) + 1));
          Serial.println(ino); 

      ino.toCharArray(charBuf, 50);
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
          display.drawString(0, 0, ino);
          display.display();
          
          display.drawString(0, 13, "Searching network...");
          display.display();

          
  AsyncWiFiManager wifiManager(&server,&dns);   // WifiManager
  wifiManager.autoConnect();                    // WifiManager
    
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);         // Wifi Connection
     
   Serial.printf("\n\nConnecting to %s\n", WIFI_SSID);
   display.drawString(0, 26, "Connecting to WiFi..");
   display.display();
                   
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }

      Serial.println("");
   display.drawString(0, 39, "Connected");
   display.display();
   delay(1500);
   display.clear(); // clear the display

      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {   // OTA
    request->send(200, "text/plain", "IOT_Home_Weather");             // OTA
    });
    
      AsyncElegantOTA.begin(&server);                                 // Start ElegantOTA
        server.begin();                                                 
          Serial.println("OTA HTTP server started");

    String text;                                                      // JSON
      text.reserve(JSON_BUFF_DIMENSION);                              // JSON

            
  // The ESP is capable of rendering 60fps in 80Mhz mode
  // but that won't give you much time for anything else
  // run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(40);
  
    // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();
  

  display.flipScreenVertically();
             
}

/***************************************************************************************
**                         Main Loop
***************************************************************************************/
void loop() {

   unsigned long currentMillis = millis();          // Timer for loops
   AsyncElegantOTA.loop();                          // OTA is verified every loop
          
   int remainingTimeBudget = ui.update();
 
           
          if (WiFi.status() == WL_CONNECTED) {     // main interaction
            if (remainingTimeBudget > 0) {
    
                if (currentMillis - Loop1previousMillis >= Loop1interval) {
                  Loop1previousMillis = currentMillis;
                    requestCurrentWiFi();                 // Check wifi condition 
                    geolocationRequest();                 // Get geolocation by IP
                    requestCurrentWeather();              // Get weather data
                }
     
            delay(remainingTimeBudget);
            }
       }
}  
//delay(postInterval);
