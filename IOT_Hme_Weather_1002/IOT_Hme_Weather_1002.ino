/*
 * 
 * 
   IOT_Home_weathes station 
   UI Display + Wifi manager + OTA + wifi Screen............. 1000 (from IOT_Hme_Wth_504)                                    
   IP localization + geo screen ............................. 1001  
   API OpenWeatherMap ....................................... 1002

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

#define TIME_OFFSET 1UL * -10800UL // UTC + 0 hour

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
   requestCurrentWeather();     

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

/***************************************************************************************
**                          Send WiFi info to serial port
***************************************************************************************/
void WiFiStatus()
{

    wifiSSID = WiFi.SSID();
    wifiIP = ipToString(WiFi.localIP());
    wifiRSSI = String(WiFi.RSSI());

    Serial.println("");Serial.println("");
    Serial.println("############### Current WIFI ###############\n");
    Serial.print("SSID             : "); Serial.println(wifiSSID);
    Serial.print("IP               : "); Serial.println(wifiIP);
    Serial.print("RSSI             : "); Serial.println(wifiRSSI);
    Serial.print("Status           : "); Serial.println(wl_status_to_string(WiFi.status()));
    Serial.println();
    
}

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}

String ipToString(IPAddress ip){
          String s="";
          for (int i=0; i<4; i++)
            s += i  ? "." + String(ip[i]) : String(ip[i]);
          return s;
}


/***************************************************************************************
**                          Geolocation by IP
***************************************************************************************/
void geolocationRequest() {
    
HTTPClient http;                                        //Declare an object of class HTTPClient
 
http.begin("http://ip-api.com/json/177.51.71.181");     // specify request destination 
//http.begin("http://ip-api.com/json/"+WiFi.localIP());
    
int httpCode = http.GET();  // send the request
 
if (httpCode > 0) { // check the returning code
 
   String input = http.getString();   //Get the request response payload
              //Serial.println("input");         //for debug
                  //Serial.println(input);

// Code from Arduino JSON assistant
          
          StaticJsonDocument<512> doc;
          
          DeserializationError error = deserializeJson(doc, input);
          
          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
          }
          
          const char* status = doc["status"]; 
          const char* country = doc["country"];
          const char* countryCode = doc["countryCode"];
          const char* region = doc["region"];
          const char* regionName = doc["regionName"];
          const char* city = doc["city"];      
          const char* zip = doc["zip"];
          float lat = doc["lat"];
          float lon = doc["lon"];
          const char* timezone = doc["timezone"];
          const char* isp = doc["isp"];
          const char* org = doc["org"];
          const char* as = doc["as"];
          const char* query = doc["query"];
          
              Serial.println("");Serial.println("");
              Serial.println("############### Geolocation by IP ###############\n");
              Serial.print("Status           : "); Serial.println(status);
              Serial.print("Country          : "); Serial.println(country);
              Serial.print("countryCode      : "); Serial.println(countryCode);
              Serial.print("region           : "); Serial.println(region);
              Serial.print("regionName       : "); Serial.println(regionName);
              Serial.print("city             : "); Serial.println(city);
              Serial.print("zip              : "); Serial.println(zip);
              Serial.print("lat              : "); Serial.println(lat);
              Serial.print("lon              : "); Serial.println(lon);
              Serial.print("timezone         : "); Serial.println(timezone);
              Serial.print("isp              : "); Serial.println(isp);
              Serial.print("org              : "); Serial.println(org);
              Serial.print("as               : "); Serial.println(as);
              Serial.print("query            : "); Serial.println(query);


/***************************************************************************************
**           Convert float to String and feed Openweather location
***************************************************************************************/                       
                          char buff[10];

                          dtostrf(lat, 2, 4, buff);  //4 is mininum width, 6 is precision
                             latitude = buff;
 
                          dtostrf(lon, 2, 4, buff);  //4 is mininum width, 6 is precision
                             longitude = buff;

                          currentCity = doc["city"].as<String>();   

      }
}

/***************************************************************************************
**                          Send weather info to serial port
***************************************************************************************/
void requestCurrentWeather(){

HTTPClient http;  //Declare an object of class HTTPClient
 
// specify request destination
http.begin("http://api.openweathermap.org/data/2.5/onecall?lat=-30.0331&lon=-51.23&exclude=hourly,minutely&units=metric&appid=b4d2a60b9952e3dd9e52a1f1196cabe6");
 
int httpCode = http.GET();  // send the request
 
if (httpCode > 0) { // check the returning code
 
      String input = http.getString();   //Get the request response payload
              //Serial.println("input");         //for debug
              //    Serial.println(input);

// Code from Arduino JSON assistant

DynamicJsonDocument doc(6144);

DeserializationError error = deserializeJson(doc, input);

          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
          }
          
          float lat = doc["lat"]; // -30.0331
          float lon = doc["lon"]; // -51.23
          const char* timezone = doc["timezone"]; // "America/Sao_Paulo"
          int timezone_offset = doc["timezone_offset"]; // -10800
          
          JsonObject current = doc["current"];
          long current_dt = current["dt"]; // 1624477971
          long current_sunrise = current["sunrise"]; // 1624443650
          long current_sunset = current["sunset"]; // 1624480405
          float current_temp = current["temp"]; // 19.35
          float current_feels_like = current["feels_like"]; // 19.19
          int current_pressure = current["pressure"]; // 1017
          int current_humidity = current["humidity"]; // 71
          float current_dew_point = current["dew_point"]; // 13.96
          float current_uvi = current["uvi"]; // 0.09
          int current_clouds = current["clouds"]; // 0
          int current_visibility = current["visibility"]; // 10000
          float current_wind_speed = current["wind_speed"]; // 1.79
          int current_wind_deg = current["wind_deg"]; // 80
          float current_wind_gust = current["wind_gust"]; // 4.92
          
          JsonObject current_weather_0 = current["weather"][0];
          int current_weather_0_id = current_weather_0["id"]; // 800
          const char* current_weather_0_main = current_weather_0["main"]; // "Clear"
          const char* current_weather_0_description = current_weather_0["description"]; // "clear sky"
          const char* current_weather_0_icon = current_weather_0["icon"]; // "01d"
          
          JsonArray daily = doc["daily"];
          
          JsonObject daily_0 = daily[0];
          long daily_0_dt = daily_0["dt"]; // 1624460400
          long daily_0_sunrise = daily_0["sunrise"]; // 1624443650
          long daily_0_sunset = daily_0["sunset"]; // 1624480405
          long daily_0_moonrise = daily_0["moonrise"]; // 1624476600
          long daily_0_moonset = daily_0["moonset"]; // 1624438620
          float daily_0_moon_phase = daily_0["moon_phase"]; // 0.46
          
          JsonObject daily_0_temp = daily_0["temp"];
          float daily_0_temp_day = daily_0_temp["day"]; // 18.29
          float daily_0_temp_min = daily_0_temp["min"]; // 12.36
          float daily_0_temp_max = daily_0_temp["max"]; // 19.75
          float daily_0_temp_night = daily_0_temp["night"]; // 15.85
          float daily_0_temp_eve = daily_0_temp["eve"]; // 18.87
          float daily_0_temp_morn = daily_0_temp["morn"]; // 12.4
          
          JsonObject daily_0_feels_like = daily_0["feels_like"];
          float daily_0_feels_like_day = daily_0_feels_like["day"]; // 17.85
          float daily_0_feels_like_night = daily_0_feels_like["night"]; // 15.61
          float daily_0_feels_like_eve = daily_0_feels_like["eve"]; // 18.72
          float daily_0_feels_like_morn = daily_0_feels_like["morn"]; // 12.1
          
          int daily_0_pressure = daily_0["pressure"]; // 1022
          int daily_0_humidity = daily_0["humidity"]; // 64
          float daily_0_dew_point = daily_0["dew_point"]; // 11.4
          float daily_0_wind_speed = daily_0["wind_speed"]; // 3.54
          int daily_0_wind_deg = daily_0["wind_deg"]; // 92
          float daily_0_wind_gust = daily_0["wind_gust"]; // 9.42
          
          JsonObject daily_0_weather_0 = daily_0["weather"][0];
          int daily_0_weather_0_id = daily_0_weather_0["id"]; // 804
          const char* daily_0_weather_0_main = daily_0_weather_0["main"]; // "Clouds"
          const char* daily_0_weather_0_description = daily_0_weather_0["description"]; // "overcast clouds"
          const char* daily_0_weather_0_icon = daily_0_weather_0["icon"]; // "04d"
          
          int daily_0_clouds = daily_0["clouds"]; // 100
          float daily_0_pop = daily_0["pop"]; // 0.04
          float daily_0_uvi = daily_0["uvi"]; // 2.76
          
          JsonObject daily_1 = daily[1];
          long daily_1_dt = daily_1["dt"]; // 1624546800
          long daily_1_sunrise = daily_1["sunrise"]; // 1624530059
          long daily_1_sunset = daily_1["sunset"]; // 1624566820
          long daily_1_moonrise = daily_1["moonrise"]; // 1624566540
          long daily_1_moonset = daily_1["moonset"]; // 1624529280
          float daily_1_moon_phase = daily_1["moon_phase"]; // 0.5
          
          JsonObject daily_1_temp = daily_1["temp"];
          float daily_1_temp_day = daily_1_temp["day"]; // 16.48
          float daily_1_temp_min = daily_1_temp["min"]; // 14.72
          float daily_1_temp_max = daily_1_temp["max"]; // 17.86
          float daily_1_temp_night = daily_1_temp["night"]; // 15.95
          float daily_1_temp_eve = daily_1_temp["eve"]; // 16.48
          float daily_1_temp_morn = daily_1_temp["morn"]; // 14.94
          
          JsonObject daily_1_feels_like = daily_1["feels_like"];
          float daily_1_feels_like_day = daily_1_feels_like["day"]; // 16.53
          float daily_1_feels_like_night = daily_1_feels_like["night"]; // 15.98
          float daily_1_feels_like_eve = daily_1_feels_like["eve"]; // 16.59
          float daily_1_feels_like_morn = daily_1_feels_like["morn"]; // 14.55
          
          int daily_1_pressure = daily_1["pressure"]; // 1012
          int daily_1_humidity = daily_1["humidity"]; // 90
          float daily_1_dew_point = daily_1["dew_point"]; // 14.93
          float daily_1_wind_speed = daily_1["wind_speed"]; // 8.22
          int daily_1_wind_deg = daily_1["wind_deg"]; // 3
          float daily_1_wind_gust = daily_1["wind_gust"]; // 16.61
          
          JsonObject daily_1_weather_0 = daily_1["weather"][0];
          int daily_1_weather_0_id = daily_1_weather_0["id"]; // 501
          const char* daily_1_weather_0_main = daily_1_weather_0["main"]; // "Rain"
          const char* daily_1_weather_0_description = daily_1_weather_0["description"]; // "moderate rain"
          const char* daily_1_weather_0_icon = daily_1_weather_0["icon"]; // "10d"
          
          int daily_1_clouds = daily_1["clouds"]; // 100
          int daily_1_pop = daily_1["pop"]; // 1
          float daily_1_rain = daily_1["rain"]; // 8.18
          float daily_1_uvi = daily_1["uvi"]; // 1.49
          
          JsonObject alerts_0 = doc["alerts"][0];
          const char* alerts_0_sender_name = alerts_0["sender_name"]; // "Instituto Nacional de Meteorologia"
          const char* alerts_0_event = alerts_0["event"]; // "Tempestade"
          long alerts_0_start = alerts_0["start"]; // 1624456800
          long alerts_0_end = alerts_0["end"]; // 1624543200
          const char* alerts_0_description = alerts_0["description"]; // "INMET publica aviso iniciando em: ...
          
          const char* alerts_0_tags_0 = alerts_0["tags"][0]; // "Wind"
          
              Serial.println("");Serial.println("");
              Serial.println("############### Current OpenWeatherMap ###############\n");
              Serial.print("Current_dt       : "); Serial.println(strTime(current_dt));
              Serial.print("Current_temp     : "); Serial.println(current_temp);
              Serial.print("Current_eels_like: "); Serial.println(current_feels_like);
              Serial.print("Current_weather  : "); Serial.println(current_weather_0_id);

              Serial.println("");Serial.println("");
              Serial.println("############### Forecast Tomorrow OpenWeatherMap ###############\n");
              Serial.print("Tomorrow_dt       : "); Serial.println(strTime(daily_0_dt));
              Serial.print("Tomorrow_temp_max : "); Serial.println(daily_0_temp_max);
              Serial.print("Tomorrow_temp_min : "); Serial.println(daily_0_temp_min);
              Serial.print("Tomorrow_weather  : "); Serial.println(daily_0_weather_0_id);
              
              Serial.println("");Serial.println("");
              Serial.println("############### Forecast Day After Tomorrow OpenWeatherMap ###############\n");
              Serial.print("DayAftTmw_dt    : "); Serial.println(strTime(daily_1_dt));
              Serial.print("DayAftTmwtempmax: "); Serial.println(daily_1_temp_max);
              Serial.print("DayAftTmwtempmin: "); Serial.println(daily_1_temp_min);
              Serial.print("Tomorrow_weather  : "); Serial.println(daily_0_weather_0_id);


    }
}

/***************************************************************************************
**                          Convert unix time to a time string
***************************************************************************************/
String strTime(time_t unixTime)
{
  unixTime += TIME_OFFSET;
  return ctime(&unixTime);
}
