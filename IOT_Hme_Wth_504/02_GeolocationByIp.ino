/***************************************************************************************
**                          Geolocation by IP
***************************************************************************************/
void geolocationRequest() {
    
    HTTPClient http_geo;  //Declare an object of class HTTPClient
 
    // specify request destination
           http_geo.begin("http://ip-api.com/json/177.51.71.181");
           //http.begin("http://ip-api.com/json/"+WiFi.localIP());
    
    int httpCode = http_geo.GET();  // send the request
 
    if (httpCode > 0) { // check the returning code
 
      String input = http_geo.getString();   //Get the request response payload
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
//const char* city = doc["city"];       // needs to be declared globally to be available for other functions
  city = doc["city"].as<String>(); 
const char* zip = doc["zip"];
//float lat = doc["lat"];
  lat = doc["lat"];                     // needs to be declared globally to be available for other functions
//float lon = doc["lon"];
  lon = doc["lon"];                     // needs to be declared globally to be available for other functions
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

                //Serial.print("String Latitude ... ");Serial.println(latitude);
                dtostrf(lat, 2, 4, buff);  //4 is mininum width, 6 is precision
                   latitude = buff;
                //   Serial.print("float Lat ......... ");Serial.println(lat,4);
                //   Serial.print("String Latitude ... ");Serial.println(latitude);

                //Serial.print("String Longitude .. ");Serial.println(longitude); 
                dtostrf(lon, 2, 4, buff);  //4 is mininum width, 6 is precision
                   longitude = buff;
                //   Serial.print("float Lon ......... ");Serial.println(lon,4);
                //   Serial.print("String Longitude .. ");Serial.println(longitude); 
      }
}
