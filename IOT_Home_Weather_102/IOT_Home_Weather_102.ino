/*
   Author: Emmanuel Odunlade, 2017
   Modified March 12, 2019, by Michael Shu
   Original project idea and code by Emmanuel Odunlade.
   Complete Project Details https://randomnerdtutorials.com/esp8266-weather-forecaster/
   - Updated code to use modern API and parsing (USE ARDUINOJSON 5.13.5)
   - Adjusted for NodeMCU 1.0

   IOT_Home_weathes station
   Weather data download ............. 100                                       
   Weather data split prototype ...... 101
   Weather data split & cleanup ...... 102
   OLED display ...................... 103
   NTC clock ......................... 104
   OTA ............................... 105

// https://openweathermap.org/api/one-call-api
// weather codes found at https://openweathermap.org/weather-conditions
   {"cod":"200","message":0,"cnt":2,"list":[{"dt":1620756000,"main":{"temp":18.56,"feels_like":17.91,"temp_min":17.66,"temp_max":18.56,"pressure":1018,"sea_level":1018,"grnd_level":1014,"humidity":55,"temp_kf":0.9},"weather":[{"id":802,"main":"Clouds","description":"scattered clouds","icon":"03d"}],"clouds":{"all":40},"wind":{"speed":2.48,"deg":152,"gust":2.84},"visibility":10000,"pop":0.14,"sys":{"pod":"d"},"dt_txt":"2021-05-11 18:00:00"},{"dt":1620766800,"main":{"temp":17.57,"feels_like":16.9,"temp_min":15.59,"temp_max":17.57,"pressure":1018,"sea_level":1018,"grnd_level":1014,"humidity":58,"temp_kf":1.98},"weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04n"}],"clouds":{"all":60},"wind":{"speed":3.18,"deg":147,"gust":5.55},"visibility":10000,"pop":0,"sys":{"pod":"n"},"dt_txt":"2021-05-11 21:00:00"}],"city":{"id":3452925,"name":"Porto Alegre","coord":{"lat":-30.0331,"lon":-51.23},"country":"BR","population":0,"timezone":-10800,"sunrise":1620727228,"sunset":1620765732}


 API call example
    https://api.openweathermap.org/data/2.5/onecall?lat=33.44&lon=-94.04&exclude=hourly,daily&appid={API key}
    https://api.openweathermap.org/data/2.5/forecast?id="3452925"&APPID="b4d2a60b9952e3dd9e52a1f1196cabe6"&mode=json&units=metric&cnt=2 HTTP/1.1
    
    https://openweathermap.org/current
    POA current weather: http://api.openweathermap.org/data/2.5/weather?id=3452925&units=metric&appid=b4d2a60b9952e3dd9e52a1f1196cabe6
                         https://api.openweathermap.org/data/2.5/onecall?lat=-30.0331&lon=-51.23&exclude=hourly,daily,minutely&units=metric&appid=b4d2a60b9952e3dd9e52a1f1196cabe6


    
// NOTE: IF THIS IS NOT WORKING FOR YOU, TRY YOUR RESET BUTTON FIRST

/***************************************************
* Libraries
****************************************************/
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

/***************************************************
* Constant definitions
****************************************************/

char ssid[] = "CasaDoTheodoro";
char pass[] = "09012011";

// Open Weather Map API server name
const char server[] = "api.openweathermap.org";

// Replace the next line to match your city code
// City codes can be found at http://bulk.openweathermap.org/sample/city.list.json.gz
//String cityID = "3452925"; //Porto Alegre
String cityID = "lat=-30.0331&lon=-51.23"; //Porto Alegre
// How your nameOfCity variable would look like for Richmond in Canada
//String cityID = "6122081";

// Replace the next line with your API Key
String apiKey = "b4d2a60b9952e3dd9e52a1f1196cabe6";

String text;
String variable;
int lenght;
String DataIn;
String inputString;

int jsonend = 0;
boolean startJson = false;
int status = WL_IDLE_STATUS;

#define JSON_BUFF_DIMENSION 2500

unsigned long lastConnectionTime = 0;               // last time you connected to the server, in milliseconds
//const unsigned long postInterval = 10 * 60 * 1000;  // posting interval of 1 minutes  (1L * 1000L; 10 seconds delay for testing)
const unsigned long postInterval = 1 * 10 * 1000;  // posting interval of 10sec  (1L * 1000L; 10 seconds delay for testing)


WiFiClient client;

/***************************************************
* Setup
****************************************************/
void setup() {
   
  Serial.begin(115200); 

  text.reserve(JSON_BUFF_DIMENSION);

  WiFi.begin(ssid, pass);
  Serial.println("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");
  readWiFiStatus();
}

/***************************************************
* Main loop
****************************************************/
void loop() {
  //OWM requires 10mins between request intervals
  //check if 10mins has passed then conect again and pull
      unsigned long currentMillis = millis();
          if (currentMillis - lastConnectionTime >= postInterval) {
            // note the time that the connection was made:
            lastConnectionTime = currentMillis;
            Serial.println("Request data");
            
            inputString = makehttpRequest();

            Serial.println();
            Serial.println();
          
/***************************************************
* Data treatment from Open Weather Maps
****************************************************/

      String searchString = "daily";  // Split weather from current and daily
          
      String beforeMatch = BeforeMatchRegularExpression(searchString, inputString);   // Current weather

      String afterMatch = AfterMatchRegularExpression(searchString, inputString);     // Weather forecast
           
/***************************************************
* Requested data from Open Weather Maps
****************************************************/
             // CURRENT WEATHER             
                          // Date
                          variable = "dt";
                            lenght = 10;
                              DataIn = stringSubSet(variable, beforeMatch, lenght);

                              unsigned long t = DataIn.toInt()-10800;
                              char buff[32];
                              sprintf(buff, "%02d.%02d.%02d %02d:%02d:%02d", day(t), month(t), year(t), hour(t), minute(t), second(t));
                              //Serial.println(buff);
                          String currentDate = DataIn;
                          Serial.print(variable);
                          Serial.print(" ............. ");
                          Serial.println(buff);
                                                   
                          // Temperature
                          variable = "temp";
                            lenght = 5;
                              DataIn = stringSubSet(variable, beforeMatch, lenght);
                                String currentTemp = DataIn;
                          Serial.print(variable);
                          Serial.print(" ........... ");
                          Serial.print(DataIn);
                          Serial.println(" ºC ");
              
                          // Feels like
                          variable = "feels_like";
                            lenght = 5;
                              DataIn = stringSubSet(variable, beforeMatch, lenght);
                                String currentFeels_like = DataIn;
                          Serial.print(variable);
                          Serial.print(" ..... ");
                          Serial.print(DataIn);
                          Serial.println(" ºC ");

                          // WiFi Status
                          Serial.print("SSID");
                          Serial.print(" ........... ");
                          Serial.print(WiFi.SSID());
                          Serial.println("");

                          Serial.print("IP");
                          Serial.print(" ............. ");
                          Serial.print(WiFi.localIP());
                          Serial.println("");
                          
                          Serial.print("RSSI");
                          Serial.print(" ........... ");
                          Serial.print(WiFi.RSSI());
                          Serial.println(" dBm");
        
  }
}

/***************************************************
* Read Wifi Status
****************************************************/

void readWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

/***************************************************
* Request data from Open Weather Maps
****************************************************/
String makehttpRequest() {
  // close any connection before send a new request to allow client make connection to server
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
     Serial.println("connecting to OWM server...");
    // send the HTTP PUT request:
    //client.println("GET /data/2.5/forecast?id=" + cityID + "&APPID=" + apiKey + "&mode=json&units=metric&cnt=2 HTTP/1.1");
    client.println("GET /data/2.5/onecall?" + cityID + "&exclude=hourly,minutely,alerts&units=metric&appid=" + apiKey + " HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();

    int timeout=0;
    while (client.available() == 0) {
      if (timeout>= 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
      }
      timeout++;
      delay(1);
    }
       
    char c = 0;
    while (client.available()) {
      c = client.read();
      text += c;
      
      delay(1); // fixes some issues some other users experienced
      }

      Serial.println("Weather raw data from API : ");  
      //Serial.println(text);  // Raw data retrieved       

      return text;
    }
}

/***************************************************
* Retrieve data from first portion of the text
****************************************************/
String BeforeMatchRegularExpression(String search, String input){
  
  String firstPortion = input.substring(0, input.indexOf(search));
    
  return firstPortion;  
}

/***************************************************
* Retrieve data from second portion of the text
****************************************************/
String AfterMatchRegularExpression(String search, String input){
  
  String secondPortion = input.substring(input.indexOf(search)+search.length(), input.length());
    
  return secondPortion;  
}

/***************************************************
* Get specified data from text
****************************************************/
String stringSubSet(String search, String input, int stringLenght){

/*
,"temp":18.43,
  1
  1234
      12
        12345
*/

  int indexStart = 
    input.indexOf(search)+    // Start index from selected word
    search.length()+          // Word lenght
    2;                        // non-recognizable characters that are present
  
  int indexEnd =
    stringLenght+
    indexStart;
  
  String stringSubSet = input.substring(indexStart, indexEnd);
    
  return stringSubSet;
}
