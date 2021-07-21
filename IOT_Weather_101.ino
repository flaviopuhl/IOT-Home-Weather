/*
 
  Baseline IOT_Hme_Weather_1007
  DS18B20 ................. 100
  OWM ..................... 101
  
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

String Tomorrow_dt;
String Tomorrow_temp_min;
String Tomorrow_temp_max;

String DayAftTmw_dt;
String DayAftTmw_temp_min;     
String DayAftTmw_temp_max;

unsigned long previousMillis1;

/*************************************************************************************
 * Setup
*************************************************************************************/
void setup() {
  
  Serial.begin(115200);                                       // Initialize Serial Monitor
    delay(3000);                                              // Delay to stabilize serial com
    
  timeClient.begin();                                         // Initialize a NTPClient to get time
  sensors.begin();                                            // Start the DS18B20 sensor
  
  getSWversion();
  wifiConnect();                                              // Connect to Wi-Fi
    getWifiStatus();
  
  ThingerDefinitions();                                       // Thinger
  
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

  
  delay(1);
}
