/* 
 *  Sketch for ESP32 to fetch the Weather Forecast from OpenWeather
 *  an example from the library here: https://github.com/Bodmer/OpenWeather
 *  
 *  Sign up for a key and read API configuration info here: 
 *  https://openweathermap.org/

   IOT_Home_weathes station
   Weather data download ............. 100  ok                                     
   OLED display ...................... 200
   Wifi Manager ............... ...... 300
   IP localization ................... 400
   NTP clock ......................... 500
   OTA ............................... 600

// https://openweathermap.org/api/one-call-api
// weather codes found at https://openweathermap.org/weather-conditions
   {"cod":"200","message":0,"cnt":2,"list":[{"dt":1620756000,"main":{"temp":18.56,"feels_like":17.91,"temp_min":17.66,"temp_max":18.56,"pressure":1018,"sea_level":1018,"grnd_level":1014,"humidity":55,"temp_kf":0.9},"weather":[{"id":802,"main":"Clouds","description":"scattered clouds","icon":"03d"}],"clouds":{"all":40},"wind":{"speed":2.48,"deg":152,"gust":2.84},"visibility":10000,"pop":0.14,"sys":{"pod":"d"},"dt_txt":"2021-05-11 18:00:00"},{"dt":1620766800,"main":{"temp":17.57,"feels_like":16.9,"temp_min":15.59,"temp_max":17.57,"pressure":1018,"sea_level":1018,"grnd_level":1014,"humidity":58,"temp_kf":1.98},"weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04n"}],"clouds":{"all":60},"wind":{"speed":3.18,"deg":147,"gust":5.55},"visibility":10000,"pop":0,"sys":{"pod":"n"},"dt_txt":"2021-05-11 21:00:00"}],"city":{"id":3452925,"name":"Porto Alegre","coord":{"lat":-30.0331,"lon":-51.23},"country":"BR","population":0,"timezone":-10800,"sunrise":1620727228,"sunset":1620765732}

// IP localization
https://ip-api.com/#177.51.71.181

//Wifi interface
https://www.hackster.io/hieromon-ikasamo/esp8266-esp32-connect-wifi-made-easy-d75f45
https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/

 API call example
    https://api.openweathermap.org/data/2.5/onecall?lat=33.44&lon=-94.04&exclude=hourly,daily&appid={API key}
    https://api.openweathermap.org/data/2.5/forecast?id="3452925"&APPID="b4d2a60b9952e3dd9e52a1f1196cabe6"&mode=json&units=metric&cnt=2 HTTP/1.1
    
    https://openweathermap.org/current
    POA current weather: http://api.openweathermap.org/data/2.5/weather?id=3452925&units=metric&appid=b4d2a60b9952e3dd9e52a1f1196cabe6
                         https://api.openweathermap.org/data/2.5/onecall?lat=-30.0331&lon=-51.23&exclude=hourly,daily,minutely&units=metric&appid=b4d2a60b9952e3dd9e52a1f1196cabe6


    
// NOTE: IF THIS IS NOT WORKING FOR YOU, TRY YOUR RESET BUTTON FIRST
*/

/***************************************************************************************
**                          Libraries
***************************************************************************************/
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
#else // ESP32
  #include <WiFi.h>
#endif

#include <JSON_Decoder.h>

#include <OpenWeather.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

// Just using this library for unix time conversion
#include <Time.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager

/***************************************************************************************
**                          User configurations
***************************************************************************************/

#define TIME_OFFSET 1UL * -10800UL // UTC + 0 hour

// Change to suit your WiFi router
#define WIFI_SSID     "CasaDoTheodoro"
#define WIFI_PASSWORD "09012011"

// OpenWeather API Details, replace x's with your API key
String api_key = "b4d2a60b9952e3dd9e52a1f1196cabe6"; // Obtain this from your OpenWeather account

// Set both your longitude and latitude to at least 4 decimal places
//String latitude =  "27.9881"; // 90.0000 to -90.0000 negative for Southern hemisphere
//String longitude = "86.9250"; // 180.000 to -180.000 negative for West

String latitude =  "-30.0331"; // 90.0000 to -90.0000 negative for Southern hemisphere
String longitude = "-51.23"; // 180.000 to -180.000 negative for West

String units = "metric";  // or "imperial"
String language = "en";   // See notes tab

unsigned long lastConnectionTime =  millis();               // last time you connected to the server, in milliseconds
const unsigned long postInterval = 5 * 60 * 1000;  // posting interval of 5 minutes  (1L * 1000L; 10 seconds delay for testing)
//const unsigned long postInterval = 1 * 10 * 1000;  // posting interval of 10sec  (1L * 1000L; 10 seconds delay for testing)

/***************************************************************************************
**                         Code starts here
***************************************************************************************/

OW_Weather ow; // Weather forecast library instance

void setup() { 
  Serial.begin(115200); // Fast to stop it holding up the stream

  Serial.printf("\n\nConnecting to %s\n", WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
   
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected\n");
}

void loop() {

    unsigned long currentMillis = millis();
          if (currentMillis - lastConnectionTime >= postInterval) {
            lastConnectionTime = currentMillis;
          
             printCurrentWeather();

          }

}

/***************************************************************************************
**                          Send weather info to serial port
***************************************************************************************/
void printCurrentWeather()
{
  // Create the structures that hold the retrieved weather
  OW_current *current = new OW_current;
  OW_hourly *hourly = new OW_hourly;
  OW_daily  *daily = new OW_daily;

  Serial.print("\nRequesting weather information from OpenWeather... ");

  //On the ESP8266 (only) the library by default uses BearSSL, another option is to use AXTLS
  //For problems with ESP8266 stability, use AXTLS by adding a false parameter thus       vvvvv
  //ow.getForecast(current, hourly, daily, api_key, latitude, longitude, units, language, false);

  ow.getForecast(current, hourly, daily, api_key, latitude, longitude, units, language);

  Serial.println("Weather from Open Weather\n");

  // Position as reported by Open Weather
  Serial.print("Latitude            : "); Serial.println(ow.lat);
  Serial.print("Longitude           : "); Serial.println(ow.lon);
  // We can use the timezone to set the offset eventually...
  Serial.print("Timezone            : "); Serial.println(ow.timezone);
  Serial.println();

  if (current)
  {
    Serial.println("############### Current weather ###############\n");
    Serial.print("dt (time)        : "); Serial.print(strTime(current->dt));
    Serial.print("sunrise          : "); Serial.print(strTime(current->sunrise));
    Serial.print("sunset           : "); Serial.print(strTime(current->sunset));
    Serial.print("temp             : "); Serial.println(current->temp);
    Serial.print("feels_like       : "); Serial.println(current->feels_like);
    Serial.print("pressure         : "); Serial.println(current->pressure);
    Serial.print("humidity         : "); Serial.println(current->humidity);
    Serial.print("dew_point        : "); Serial.println(current->dew_point);
    Serial.print("uvi              : "); Serial.println(current->uvi);
    Serial.print("clouds           : "); Serial.println(current->clouds);
    Serial.print("visibility       : "); Serial.println(current->visibility);
    Serial.print("wind_speed       : "); Serial.println(current->wind_speed);
    Serial.print("wind_gust        : "); Serial.println(current->wind_gust);
    Serial.print("wind_deg         : "); Serial.println(current->wind_deg);
    Serial.print("rain             : "); Serial.println(current->rain);
    Serial.print("snow             : "); Serial.println(current->snow);
    Serial.println();
    Serial.print("id               : "); Serial.println(current->id);
    Serial.print("main             : "); Serial.println(current->main);
    Serial.print("description      : "); Serial.println(current->description);
    Serial.print("icon             : "); Serial.println(current->icon);

    Serial.println();
  }

  if (hourly)
  {
    Serial.println("############### Hourly weather  ###############\n");
    for (int i = 0; i < MAX_HOURS; i++)
    {
      Serial.print("Hourly summary  "); if (i < 10) Serial.print(" "); Serial.print(i);
      Serial.println();
      Serial.print("dt (time)        : "); Serial.print(strTime(hourly->dt[i]));
      Serial.print("temp             : "); Serial.println(hourly->temp[i]);
      Serial.print("feels_like       : "); Serial.println(hourly->feels_like[i]);
      Serial.print("pressure         : "); Serial.println(hourly->pressure[i]);
      Serial.print("humidity         : "); Serial.println(hourly->humidity[i]);
      Serial.print("dew_point        : "); Serial.println(hourly->dew_point[i]);
      Serial.print("clouds           : "); Serial.println(hourly->clouds[i]);
      Serial.print("wind_speed       : "); Serial.println(hourly->wind_speed[i]);
      Serial.print("wind_gust        : "); Serial.println(hourly->wind_gust[i]);
      Serial.print("wind_deg         : "); Serial.println(hourly->wind_deg[i]);
      Serial.print("rain             : "); Serial.println(hourly->rain[i]);
      Serial.print("snow             : "); Serial.println(hourly->snow[i]);
      Serial.println();
      Serial.print("id               : "); Serial.println(hourly->id[i]);
      Serial.print("main             : "); Serial.println(hourly->main[i]);
      Serial.print("description      : "); Serial.println(hourly->description[i]);
      Serial.print("icon             : "); Serial.println(hourly->icon[i]);
      Serial.print("pop              : "); Serial.println(hourly->pop[i]);

      Serial.println();
    }
  }

  if (daily)
  {
    Serial.println("###############  Daily weather  ###############\n");
    for (int i = 0; i < MAX_DAYS; i++)
    {
      Serial.print("Daily summary   "); if (i < 10) Serial.print(" "); Serial.print(i);
      Serial.println();
      Serial.print("dt (time)        : "); Serial.print(strTime(daily->dt[i]));
      Serial.print("sunrise          : "); Serial.print(strTime(daily->sunrise[i]));
      Serial.print("sunset           : "); Serial.print(strTime(daily->sunset[i]));

      Serial.print("temp.morn        : "); Serial.println(daily->temp_morn[i]);
      Serial.print("temp.day         : "); Serial.println(daily->temp_day[i]);
      Serial.print("temp.eve         : "); Serial.println(daily->temp_eve[i]);
      Serial.print("temp.night       : "); Serial.println(daily->temp_night[i]);
      Serial.print("temp.min         : "); Serial.println(daily->temp_min[i]);
      Serial.print("temp.max         : "); Serial.println(daily->temp_max[i]);

      Serial.print("feels_like.morn  : "); Serial.println(daily->feels_like_morn[i]);
      Serial.print("feels_like.day   : "); Serial.println(daily->feels_like_day[i]);
      Serial.print("feels_like.eve   : "); Serial.println(daily->feels_like_eve[i]);
      Serial.print("feels_like.night : "); Serial.println(daily->feels_like_night[i]);

      Serial.print("pressure         : "); Serial.println(daily->pressure[i]);
      Serial.print("humidity         : "); Serial.println(daily->humidity[i]);
      Serial.print("dew_point        : "); Serial.println(daily->dew_point[i]);
      Serial.print("uvi              : "); Serial.println(daily->uvi[i]);
      Serial.print("clouds           : "); Serial.println(daily->clouds[i]);
      Serial.print("visibility       : "); Serial.println(daily->visibility[i]);
      Serial.print("wind_speed       : "); Serial.println(daily->wind_speed[i]);
      Serial.print("wind_gust        : "); Serial.println(daily->wind_gust[i]);
      Serial.print("wind_deg         : "); Serial.println(daily->wind_deg[i]);
      Serial.print("rain             : "); Serial.println(daily->rain[i]);
      Serial.print("snow             : "); Serial.println(daily->snow[i]);
      Serial.println();
      Serial.print("id               : "); Serial.println(daily->id[i]);
      Serial.print("main             : "); Serial.println(daily->main[i]);
      Serial.print("description      : "); Serial.println(daily->description[i]);
      Serial.print("icon             : "); Serial.println(daily->icon[i]);
      Serial.print("pop              : "); Serial.println(daily->pop[i]);

      Serial.println();
    }
  }

  // Delete to free up space and prevent fragmentation as strings change in length
  delete current;
  delete hourly;
  delete daily;
}

/***************************************************************************************
**                          Convert unix time to a time string
***************************************************************************************/
String strTime(time_t unixTime)
{
  unixTime += TIME_OFFSET;
  return ctime(&unixTime);
}
