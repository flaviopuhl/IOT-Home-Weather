void NPTrequest(){
  
  timeClient.setTimeOffset(-10800);                           // Set offset time in seconds to adjust timezone
  timeClient.update();                                        // NPT

  //Get a time structure
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon+1;
    int currentYear = ptm->tm_year+1900;
  
    int currentHour = ptm->tm_hour;
    int currentMin = ptm->tm_min;
    int currentSec = ptm->tm_sec;


  currentTime = String(currentHour)+                         // Must be declared as global
                       ":" +
                       String(currentMin)+
                       ":" +
                       String(currentSec); 

  currentDate = String(monthDay)+                           // Must be declared as global
                       "-" +
                       String(currentMonth)+
                       "-" +
                       String(currentYear); 

  currentFullDate = currentDate +" "+currentTime;           // Must be declared as global                    
  
  Serial.println(F(" "));
  Serial.println(F("############### Current Time ###############\n"));
  Serial.print(F("Epoch Time        : ")); Serial.println(epochTime);
  Serial.print(F("Current Date      : "));  Serial.println(currentDate);
  Serial.print(F("Current Time      : "));  Serial.println(currentTime);
  Serial.print(F("Current Full Date : "));  Serial.println(currentFullDate);

  if(currentYear<=2015){
    currentTime = "Failed to Update";
    currentDate = "Failed to Update";
  }
 
}
