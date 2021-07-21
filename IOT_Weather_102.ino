/*
 
  Baseline IOT_Hme_Weather_1007
  DS18B20 ................. 100
  OWM ..................... 101
  OLED .................... 102
  
  NPT
    https://RandomNerdTutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
  
  
*/

/*************************************************************************************
 * Libraries
*************************************************************************************/
#include <ESP8266WiFi.h>                                      // Wifi for ESP8266
#include <ThingerESP8266.h>                                   // Thinger


#include <NTPClient.h>                                        // NPT
#include <WiFiUdp.h>                                          // NPT

#include <OneWire.h>                                          //DS18B20
#include <DallasTemperature.h>                                //DS18B20

#include <ESP8266HTTPClient.h>                                // API access
#include <ArduinoJson.h>                                      // JSON decoding library

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
String Current_weather;

String Tomorrow_temp_min;
String Tomorrow_temp_max;

String DayAftTmw_temp_min;     
String DayAftTmw_temp_max;

unsigned long previousMillis1;

SSD1306Wire display(0x3c, D2, D1);                           // OLED
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

    delay(2000);
    display.clear();
  
}

/*************************************************************************************
 * Main
*************************************************************************************/

void loop() {

  unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= 30000) {          // FAST LOOP
            previousMillis = currentMillis;
            
            getWifiStatus();                                // get Wifi status
            NPTrequest();                                   // Update NPT time
            getIntTemp();                                   // Read temp from DS18B20 
            thing.handle();                                 // Thinger
    }


    if (currentMillis - previousMillis1 >= 60000) {         // SLOW LOOP
            previousMillis1 = currentMillis;
      
            getOWMforecast();                               // Update forecast from API
            
    }

    
    if (currentMillis - previousMillis3 >= 1000) {          // VERY SLOW LOOP 1s
            previousMillis3 = currentMillis;

            displayCounter++;
    }


        display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0, "Current");
        display.drawString(15, 13, "EXT");
        display.drawString(70, 13, "FEEL");
        //display.drawString(70, 25, Current_feels_like+ " ºC");
      
      //display.setFont(Dialog_plain_40);           // Show ext temp
        display.drawString(0, 19, Current_temp);
      
      //display.setFont(Dialog_plain_16);
        display.drawString(47, 40, "°C");                         

    display.setTextAlignment(TEXT_ALIGN_RIGHT);
      //display.drawString(128, 0, currentDate);
      
    display.display(); 


 // delay(1);
}
