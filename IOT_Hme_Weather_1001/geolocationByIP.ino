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
