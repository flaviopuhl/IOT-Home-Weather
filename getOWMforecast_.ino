void getOWMforecast(){

  char buff[10];
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
  //Serial.print("Latitude            : "); Serial.println(ow.lat);
  //Serial.print("Longitude           : "); Serial.println(ow.lon);
  // We can use the timezone to set the offset eventually...
  //Serial.print("Timezone            : "); Serial.println(ow.timezone);
  Serial.println();

  if (current)
  {
    
    //Serial.print("dt (time)        : "); Serial.print(strTime(current->dt));
    //Serial.print("sunrise          : "); Serial.print(strTime(current->sunrise));
    //Serial.print("sunset           : "); Serial.print(strTime(current->sunset));
    //Serial.print("temp             : "); Serial.println(current->temp);
    //Serial.print("feels_like       : "); Serial.println(current->feels_like);
    //Serial.print("pressure         : "); Serial.println(current->pressure);
    //Serial.print("humidity         : "); Serial.println(current->humidity);
    //Serial.print("dew_point        : "); Serial.println(current->dew_point);
    //Serial.print("uvi              : "); Serial.println(current->uvi);
    //Serial.print("clouds           : "); Serial.println(current->clouds);
    //Serial.print("visibility       : "); Serial.println(current->visibility);
    //Serial.print("wind_speed       : "); Serial.println(current->wind_speed);
    //Serial.print("wind_gust        : "); Serial.println(current->wind_gust);
    //Serial.print("wind_deg         : "); Serial.println(current->wind_deg);
    //Serial.print("rain             : "); Serial.println(current->rain);
    //Serial.print("snow             : "); Serial.println(current->snow);
    //Serial.println();
    //Serial.print("id               : "); Serial.println(current->id);
    //Serial.print("main             : "); Serial.println(current->main);
    //Serial.print("description      : "); Serial.println(current->description);
    //Serial.print("icon             : "); Serial.println(current->icon);

    //Serial.println();
  }

  if (daily)
  {
    //Serial.println("###############  Daily weather  ###############\n");
    //for (int i = 0; i < MAX_DAYS; i++)
    for (int i = 0; i < 2; i++)
    {
      //Serial.print("Daily summary   "); if (i < 10) Serial.print(" "); Serial.print(i);
      //Serial.println();
      //Serial.print("dt (time)        : "); Serial.print(strTime(daily->dt[i]));
      //Serial.print("sunrise          : "); Serial.print(strTime(daily->sunrise[i]));
      //Serial.print("sunset           : "); Serial.print(strTime(daily->sunset[i]));

      //Serial.print("temp.morn        : "); Serial.println(daily->temp_morn[i]);
      //Serial.print("temp.day         : "); Serial.println(daily->temp_day[i]);
      //Serial.print("temp.eve         : "); Serial.println(daily->temp_eve[i]);
      //Serial.print("temp.night       : "); Serial.println(daily->temp_night[i]);
      //Serial.print("temp.min         : "); Serial.println(daily->temp_min[i]);
      //Serial.print("temp.max         : "); Serial.println(daily->temp_max[i]);

      //Serial.print("feels_like.morn  : "); Serial.println(daily->feels_like_morn[i]);
      //Serial.print("feels_like.day   : "); Serial.println(daily->feels_like_day[i]);
      //Serial.print("feels_like.eve   : "); Serial.println(daily->feels_like_eve[i]);
      //Serial.print("feels_like.night : "); Serial.println(daily->feels_like_night[i]);

      //Serial.print("pressure         : "); Serial.println(daily->pressure[i]);
      //Serial.print("humidity         : "); Serial.println(daily->humidity[i]);
      //Serial.print("dew_point        : "); Serial.println(daily->dew_point[i]);
      //Serial.print("uvi              : "); Serial.println(daily->uvi[i]);
      //Serial.print("clouds           : "); Serial.println(daily->clouds[i]);
      //Serial.print("visibility       : "); Serial.println(daily->visibility[i]);
      //Serial.print("wind_speed       : "); Serial.println(daily->wind_speed[i]);
      //Serial.print("wind_gust        : "); Serial.println(daily->wind_gust[i]);
      //Serial.print("wind_deg         : "); Serial.println(daily->wind_deg[i]);
      //Serial.print("rain             : "); Serial.println(daily->rain[i]);
      //Serial.print("snow             : "); Serial.println(daily->snow[i]);
      //Serial.println();
      //Serial.print("id               : "); Serial.println(daily->id[i]);
      //Serial.print("main             : "); Serial.println(daily->main[i]);
      //Serial.print("description      : "); Serial.println(daily->description[i]);
      //Serial.print("icon             : "); Serial.println(daily->icon[i]);
      //Serial.print("pop              : "); Serial.println(daily->pop[i]);

      //Serial.println();
    }
  }

      if(current->temp==0&&current->feels_like==0){delay(1);}                               // If OWM update fails, keep last data
        else{
          Serial.println("############### Current weather ###############\n");
            Current_temp = dtostrf(current->temp, 2, 1, buff);
            Current_feels_like = dtostrf(current->feels_like, 2, 0, buff);
              Serial.print("dt (time)        : "); Serial.print(strTime(current->dt));
              Serial.print("temp             : "); Serial.println(Current_temp);
              Serial.print("feels_like       : "); Serial.println(Current_feels_like);
      
          Serial.println("##############  Daily weather  1 ##############\n");
            Tomorrow_temp_min=dtostrf(daily->temp_min[1], 2, 1, buff);
            Tomorrow_temp_max=dtostrf(daily->temp_max[1], 2, 1, buff);
              Serial.print("dt (time)        : "); Serial.print(strTime(daily->dt[1]));
              Serial.print("temp.min         : "); Serial.println(Tomorrow_temp_min);
              Serial.print("temp.max         : "); Serial.println(Tomorrow_temp_max);
      
          Serial.println("##############  Daily weather  2 ##############\n");
            DayAftTmw_temp_min=dtostrf(daily->temp_min[2], 2, 1, buff);    
            DayAftTmw_temp_max=dtostrf(daily->temp_max[2], 2, 1, buff);
              Serial.print("dt (time)        : "); Serial.print(strTime(daily->dt[2]));
              Serial.print("temp.min         : "); Serial.println(DayAftTmw_temp_min);
              Serial.print("temp.max         : "); Serial.println(DayAftTmw_temp_max);
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
  unixTime += -3UL * 3600UL;
  return ctime(&unixTime);
}
