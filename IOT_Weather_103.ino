/*
 
  Baseline IOT_Hme_Weather_1007
  DS18B20 ................. 100
  OWM ..................... 101
  OLED .................... 102
  OWM library ............. 103
  
*/

/*************************************************************************************
 * Libraries
*************************************************************************************/
#define _DISABLE_TLS_                                         // Needed for Thinger

#include <ESP8266WiFi.h>                                      // Wifi for ESP8266
#include <ThingerESP8266.h>                                   // Thinger
#include <WiFiClientSecure.h>

#include <NTPClient.h>                                        // NPT
#include <WiFiUdp.h>                                          // NPT

#include <OneWire.h>                                          //DS18B20
#include <DallasTemperature.h>                                //DS18B20
#include <Time.h>

#include <JSON_Decoder.h>                                     // JSON decoding library
#include <OpenWeather.h>                                      // API access


#include <Wire.h>                                             // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"                                      // legacy: #include "SSD1306.h"

/*************************************************************************************
 * Declaring prototypes (needed when using Arduino IDE tabs)
*************************************************************************************/

void getSWversion();
void wifiConnect();  
void getWifiStatus();
void ThingerDefinitions();
void getOWMforecast();
void Screen_Actual();
void Screen_TempInt();
void Screen_Forecast();

/*************************************************************************************
 * Global definitions
*************************************************************************************/

const char *ssid     = "CasaDoTheodoro";                      // Wifi
const char *password = "09012011";

WiFiUDP ntpUDP;                                               // NPT 
NTPClient timeClient(ntpUDP, "pool.ntp.org");                 // NPT

String wifiSSID;                                              // getWifiStatus()
String wifiIP;                                                // getWifiStatus()
int wifiRSSI;                                                 // getWifiStatus()

String currentTime;                                           // NPTrequest()
String currentDate;                                           // NPTrequest()
String currentFullDate;                                       // NPTrequest()

String ino = __FILE__;                                        // getSWversion()

#define user "fpuhl"                                          // Thinger
#define device_Id "IOT_HOME_WEA_dev"                          // Thinger
#define device_credentials "an1rS0Disqu_OI+J"                 // Thinger
ThingerESP8266 thing(user, device_Id, device_credentials);    // Thinger


const int oneWireBus = 14;                                    // GPIO where the DS18B20 is connected to
String string_tempC;
float float_tempC;
OneWire oneWire(oneWireBus);                                  // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);                          // Pass our oneWire reference to Dallas Temperature sensor 

unsigned long previousMillis;

String Current_temp;                                          // OWM
String Current_feels_like;

String Tomorrow_temp_min;
String Tomorrow_temp_max;

String DayAftTmw_temp_min;     
String DayAftTmw_temp_max;

String api_key = "b4d2a60b9952e3dd9e52a1f1196cabe6";          // OpenWeather API Details, replace x's with your API key
String latitude =  "-30.0331";                                // Set both your longitude and latitude to at least 4 decimal places
String longitude = "-51.23"; 
String units = "metric"; 
String language = "en"; 
OW_Weather ow;                                                // Weather forecast library instance
unsigned long previousMillis1;

SSD1306Wire display(0x3c, D2, D1);                            // OLED
unsigned long previousMillis3;
int displayCounter = 0;

/*************************************************************************************
 * Setup
*************************************************************************************/

void setup() {
  
  Serial.begin(115200);                                       // Initialize Serial Monitor
    delay(3000);                                              // Delay to stabilize serial com
    
  timeClient.begin();                                         // Initialize a NTPClient to get time
  sensors.begin();                                            // Start the DS18B20 sensor
  display.init();                                             // OLED
    display.flipScreenVertically();
    display.clear();   
  
  getSWversion();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, ino);                          // Display sw version
      display.display();
      display.drawString(0, 13, "Starting WiFi .... ");
    display.display();
      
  wifiConnect();                                              // Connect to Wi-Fi
      
      display.drawString(0, 26, "Wifi connected .... ");
    display.display();
    
  ThingerDefinitions();                                       // Thinger

            getOWMforecast();                                 // get forecast
            getWifiStatus();                                  // get Wifi status
            NPTrequest();                                     // Update NPT time
            getIntTemp();                                     // Read temp from DS18B20 

    delay(2000);
    display.clear();
  
}

/*************************************************************************************
 * Main
*************************************************************************************/

void loop() {

  unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= (1*60*1000)) {    // FAST LOOP
            previousMillis = currentMillis;

            getWifiStatus();                                // get Wifi status
            NPTrequest();                                   // Update NPT time
            getIntTemp();                                   // Read temp from DS18B20 
            thing.handle();                                 // Thinger
    }


    if (currentMillis - previousMillis1 >= (10*60*1000)) {  // SLOW LOOP
            previousMillis1 = currentMillis;
      
            getOWMforecast();                               // Update forecast from API
            
    }

    
    if (currentMillis - previousMillis3 >= 3000) {          // Screen Loop
            previousMillis3 = currentMillis;

        displayCounter++;
        
        if(displayCounter==0){Screen_Actual();}
        if(displayCounter==1){Screen_TempInt(); }
        if(displayCounter==2){Screen_Forecast(); displayCounter=-1;}      
        
    }

}
