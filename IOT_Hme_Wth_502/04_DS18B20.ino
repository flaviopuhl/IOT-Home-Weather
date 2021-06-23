/***************************************************************************************
**                     Internal temperature from DS18B20
***************************************************************************************/

void tempDS18B20() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  //float temperatureF = sensors.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  //Serial.print(temperatureF);
  //Serial.println("ºF");
  delay(100);
}
