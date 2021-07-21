void getOWMforecast(){

HTTPClient http;  //Declare an object of class HTTPClient

String latitude =  "-30.0331";                 // Porto Alegre
String longitude = "-51.23"; 
char buff[10];

http.begin("http://api.openweathermap.org/data/2.5/onecall?lat="+latitude+"&lon="+longitude+"&exclude=hourly,minutely&units=metric&appid=b4d2a60b9952e3dd9e52a1f1196cabe6");
 
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

          JsonObject daily_2 = daily[2];
          long daily_2_dt = daily_2["dt"]; // 1624546800
          long daily_2_sunrise = daily_2["sunrise"]; // 1624530059
          long daily_2_sunset = daily_2["sunset"]; // 1624566820
          long daily_2_moonrise = daily_2["moonrise"]; // 1624566540
          long daily_2_moonset = daily_2["moonset"]; // 1624529280
          float daily_2_moon_phase = daily_2["moon_phase"]; // 0.5
          
          JsonObject daily_2_temp = daily_2["temp"];
          float daily_2_temp_day = daily_2_temp["day"]; // 16.48
          float daily_2_temp_min = daily_2_temp["min"]; // 14.72
          float daily_2_temp_max = daily_2_temp["max"]; // 17.86
          float daily_2_temp_night = daily_2_temp["night"]; // 15.95
          float daily_2_temp_eve = daily_2_temp["eve"]; // 16.48
          float daily_2_temp_morn = daily_2_temp["morn"]; // 14.94
          
          JsonObject daily_2_feels_like = daily_2["feels_like"];
          float daily_2_feels_like_day = daily_2_feels_like["day"]; // 16.53
          float daily_2_feels_like_night = daily_2_feels_like["night"]; // 15.98
          float daily_2_feels_like_eve = daily_2_feels_like["eve"]; // 16.59
          float daily_2_feels_like_morn = daily_2_feels_like["morn"]; // 14.55
          
          int daily_2_pressure = daily_2["pressure"]; // 1012
          int daily_2_humidity = daily_2["humidity"]; // 90
          float daily_2_dew_point = daily_2["dew_point"]; // 14.93
          float daily_2_wind_speed = daily_2["wind_speed"]; // 8.22
          int daily_2_wind_deg = daily_2["wind_deg"]; // 3
          float daily_2_wind_gust = daily_2["wind_gust"]; // 16.61
          
          JsonObject daily_2_weather_0 = daily_2["weather"][0];
          int daily_2_weather_0_id = daily_2_weather_0["id"]; // 501
          const char* daily_2_weather_0_main = daily_2_weather_0["main"]; // "Rain"
          const char* daily_2_weather_0_description = daily_2_weather_0["description"]; // "moderate rain"
          const char* daily_2_weather_0_icon = daily_2_weather_0["icon"]; // "10d"
          
          int daily_2_clouds = daily_2["clouds"]; // 100
          int daily_2_pop = daily_2["pop"]; // 1
          float daily_2_rain = daily_2["rain"]; // 8.18
          float daily_2_uvi = daily_2["uvi"]; // 1.49
       
          JsonObject alerts_0 = doc["alerts"][0];
          const char* alerts_0_sender_name = alerts_0["sender_name"]; // "Instituto Nacional de Meteorologia"
          const char* alerts_0_event = alerts_0["event"]; // "Tempestade"
          long alerts_0_start = alerts_0["start"]; // 1624456800
          long alerts_0_end = alerts_0["end"]; // 1624543200
          const char* alerts_0_description = alerts_0["description"]; // "INMET publica aviso iniciando em: ...
          
          const char* alerts_0_tags_0 = alerts_0["tags"][0]; // "Wind"
          
              Serial.println("");Serial.println("");
              Serial.println("############### Current OpenWeatherMap ###############\n");
//              Serial.print("Current_dt       : "); Serial.println(strTime(current_dt)); 
              Serial.print("Current_temp     : "); Serial.println(current_temp);
              Serial.print("Current_eels_like: "); Serial.println(current_feels_like);
              Serial.print("Current_weather  : "); Serial.println(current_weather_0_id);

//                        Current_dt=strTime(current_dt).substring(0, 10);
                        Current_temp=dtostrf(current_temp, 2, 0, buff);               //dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
                        Current_feels_like=dtostrf(current_feels_like, 2, 1, buff);   //4 is mininum width, 6 is precision;
                        Current_weather=dtostrf(current_weather_0_id, 2, 3, buff);    //4 is mininum width, 6 is precision;
              
              Serial.println("");Serial.println("");
              Serial.println("############### Forecast Tomorrow OpenWeatherMap ###############\n");
//              Serial.print("Tomorrow_dt       : "); Serial.println(strTime(daily_1_dt));
              Serial.print("Tomorrow_temp_max : "); Serial.println(daily_1_temp_max);
              Serial.print("Tomorrow_temp_min : "); Serial.println(daily_1_temp_min);
              Serial.print("Tomorrow_weather  : "); Serial.println(daily_1_weather_0_id);

//                        Tomorrow_dt=strTime(daily_1_dt).substring(0, 10);
                        Tomorrow_temp_min=dtostrf(daily_1_temp_min, 2, 1, buff);      //dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
                        Tomorrow_temp_max=dtostrf(daily_1_temp_max, 2, 1, buff);      //dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
              
              Serial.println("");Serial.println("");
              Serial.println("############### Forecast Day After Tomorrow OpenWeatherMap ###############\n");
//              Serial.print("DayAftTmw_dt    : "); Serial.println(strTime(daily_2_dt));
              Serial.print("DayAftTmwtempmax: "); Serial.println(daily_2_temp_max);
              Serial.print("DayAftTmwtempmin: "); Serial.println(daily_2_temp_min);
              Serial.print("Tomorrow_weather  : "); Serial.println(daily_2_weather_0_id);

//                        DayAftTmw_dt=strTime(daily_2_dt).substring(0, 10);
                        DayAftTmw_temp_min=dtostrf(daily_2_temp_min, 2, 1, buff);     
                        DayAftTmw_temp_max=dtostrf(daily_2_temp_max, 2, 1, buff);    

    }
}
