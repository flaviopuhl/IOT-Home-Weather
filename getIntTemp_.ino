void getIntTemp(){

  char buff[10];
  
  sensors.requestTemperatures();                            // get Temperature from DS18B20
  float_tempC = sensors.getTempCByIndex(0);
    
    dtostrf(float_tempC, 2, 1, buff);  //2 is mininum width, 1 is precision
      string_tempC = buff;
      
  Serial.println(F(" "));
  Serial.println(F("################# INT Temp #################\n"));
  Serial.print(F("Internal Temp °C  : ")); Serial.println(float_tempC);
  
}
