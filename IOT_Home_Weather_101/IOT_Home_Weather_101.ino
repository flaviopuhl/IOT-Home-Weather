/*
   Author: Emmanuel Odunlade, 2017
   Modified March 12, 2019, by Michael Shu
   Original project idea and code by Emmanuel Odunlade.
   Complete Project Details https://randomnerdtutorials.com/esp8266-weather-forecaster/
   - Updated code to use modern API and parsing (USE ARDUINOJSON 5.13.5)
   - Adjusted for NodeMCU 1.0

   IOT_Home_weathes station
   Weather data download ............. 100
   OLED display ...................... 101
   NTC clock ......................... 102
   OTA ............................... 103

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
    makehttpRequest();
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
void makehttpRequest() {
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
        return;
      }
      timeout++;
      delay(1);
    }
        Serial.println("******Debug 145*******");
    char c = 0;
    while (client.available()) {
      c = client.read();
      // since json contains equal number of open and close curly brackets, this means we can determine when a json is completely received  by counting
      // the open and close occurences,
      //Serial.print(c);
      //if (c == '{') {
      //  startJson = true;         // set startJson true to indicate json message has started
      //  jsonend++;
      //}
      //else if (c == '}') {
      //  jsonend--;
      //}
      //if (startJson == true) {
        text += c;
      //Serial.println(c);
        delay(1); // fixes some issues some other users experienced
      }
      
      // if jsonend = 0 then we have have received equal number of curly braces
      //if (jsonend == 0 && startJson == true) {
Serial.println("***Probe 1***");  
        Serial.println(text);  // Raw data retrieved
Serial.println("***Probe 2***");
        String searchString = "daily";
        String inputString = text;
         
        Serial.println("");
        Serial.println("");
Serial.println("***Probe 3***");  
            String beforeMatch = BeforeMatchRegularExpression(searchString, inputString);
Serial.println("***Probe 4***");            
            String afterMatch = AfterMatchRegularExpression(searchString, inputString);
Serial.println("***Probe 5***");            
            Serial.println(beforeMatch);
Serial.println("***Probe 6***");            
            Serial.println(afterMatch);
Serial.println("***Probe 7***");
            variable = "temp";
            lenght = 5;
            DataIn = stringSubSet(variable, beforeMatch, lenght);
            Serial.print(variable);
            Serial.print(" : ");
            Serial.println(DataIn);
Serial.println("***Probe 8***");
            variable = "feels_like";
            lenght = 5;
            DataIn = stringSubSet(variable, beforeMatch, lenght);
            Serial.print(variable);
            Serial.print(" : ");
            Serial.println(DataIn);
Serial.println("***Probe 9***");
        
/*        
        // if parsing is broken, try putting the output from serial into https://arduinojson.org/v5/assistant/
        const size_t capacity = 2 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + 2 * JSON_OBJECT_SIZE(0) + 4 * JSON_OBJECT_SIZE(1) + 3 * JSON_OBJECT_SIZE(2) + 3 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 4 * JSON_OBJECT_SIZE(8) + 740;
        DynamicJsonBuffer jsonBuffer(capacity);

        // sample json below that can be used for debugging
        const char* json = "{\"cod\":\"200\",\"message\":0.0051,\"cnt\":2,\"list\":[{\"dt\":1552446000,\"main\":{\"temp\":-5.41,\"temp_min\":-5.41,\"temp_max\":-4.18,\"pressure\":1024.8,\"sea_level\":1024.8,\"grnd_level\":1012.59,\"humidity\":76,\"temp_kf\":-1.24},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01n\"}],\"clouds\":{\"all\":0},\"wind\":{\"speed\":2.8,\"deg\":223.501},\"snow\":{},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2019-03-13 03:00:00\"},{\"dt\":1552456800,\"main\":{\"temp\":-5.63,\"temp_min\":-5.63,\"temp_max\":-4.7,\"pressure\":1024.61,\"sea_level\":1024.61,\"grnd_level\":1012.43,\"humidity\":75,\"temp_kf\":-0.93},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01n\"}],\"clouds\":{\"all\":0},\"wind\":{\"speed\":2.23,\"deg\":230.501},\"snow\":{},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2019-03-13 06:00:00\"}],\"city\":{\"id\":6122081,\"name\":\"Richmond\",\"coord\":{\"lat\":45.1834,\"lon\":-75.8327},\"country\":\"CA\"}}";

        JsonObject& root = jsonBuffer.parseObject(text);
        
        JsonObject& list_1 = root["list"][1];

        JsonObject& list_1_weather_0 = list_1["weather"][0];
        int list_1_weather_0_id = list_1_weather_0["id"];


        text = "";                // clear text string for the next time
*/        
      //  startJson = false;        // set startJson to false to indicate that a new message has not yet started
     
      //}
      //Serial.println(text);
    }
    
  }
//  else {
    // if no connction was made:
//    Serial.println("connection failed");
//    return;
//  }
//}






String BeforeMatchRegularExpression(String search, String input){
  
  //Serial.println("**Debug SubRoutine**");
  
  //Serial.print("Text to search: ");
  //Serial.print(search);
  //Serial.print("   - Lenght: ");
  //Serial.println(search.length());
  
  
  //Serial.print("Input text: ");
  //Serial.print(input);
  //Serial.print("     - Lenght: ");
  //Serial.println(input.length());
  
  
  //Serial.print("IndexOf reference: ");
  //Serial.println(input.indexOf(search));
  
  String firstPortion = input.substring(0, input.indexOf(search));
  //Serial.print("First portion: ");
  //Serial.println(firstPortion);
  
  //String secondPortion = input.substring(input.indexOf(search)+search.length(), input.length());
  //Serial.print("Second portion: ");
  //Serial.println(secondPortion);
  
  //Serial.println("**Debug SubRoutine**");
  
  return firstPortion;
  
}



String AfterMatchRegularExpression(String search, String input){
  
  //Serial.println("**Debug SubRoutine**");
  
  //Serial.print("Text to search: ");
  //Serial.print(search);
  //Serial.print("   - Lenght: ");
  //Serial.println(search.length());
  
  
  //Serial.print("Input text: ");
  //Serial.print(input);
  //Serial.print("     - Lenght: ");
  //Serial.println(input.length());
  
  
  //Serial.print("IndexOf reference: ");
  //Serial.println(input.indexOf(search));
  
  //String firstPortion = input.substring(0, input.indexOf(search));
  //Serial.print("First portion: ");
  //Serial.println(firstPortion);
  
  String secondPortion = input.substring(input.indexOf(search)+search.length(), input.length());
  //Serial.print("Second portion: ");
  //Serial.println(secondPortion);
  
  //Serial.println("**Debug SubRoutine**");
  
  return secondPortion;
  
}


String stringSubSet(String search, String input, int stringLenght){
  
  //Serial.println("**Debug SubRoutine**");
  
  //Serial.print("Variable to search: ");
  //Serial.print(search);
  //Serial.print("   - Lenght: ");
  //Serial.println(search.length());
  
  
  //Serial.print("Input text: ");
  //Serial.print(input);
  //Serial.print("     - Lenght: ");
  //Serial.println(input.length());
  
  
  //Serial.print("IndexOf reference: ");
  //Serial.println(input.indexOf(search));
  
  //String firstPortion = input.substring(0, input.indexOf(search));
  //Serial.print("First portion: ");
  //Serial.println(firstPortion);
  
  int indexStart = 
    input.indexOf(search)+
    search.length()+
    2;
  
  int indexEnd =
    stringLenght+
    indexStart;
  
  //Serial.print("Start :");
  //Serial.println(indexStart);
    
  //Serial.print("Ends :");
  //Serial.println(indexEnd);
  
  String stringSubSet = input.substring(indexStart, indexEnd);
  
  //int indexSubset = input.indexOf(search)+search.length();
  //Serial.print("Index SubSet :");
  //Serial.println(indexSubset);
  
  //Serial.print("Subset Lenght :");
  //Serial.println(stringLenght);
  
  //Serial.print("String SubSet :");
  //Serial.println(stringSubSet);
  
  //Serial.println("**Debug SubRoutine**");
  
  return stringSubSet;
  
}
