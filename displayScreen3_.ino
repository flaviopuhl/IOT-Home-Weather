void Screen_Forecast(){

display.clear();

        display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0, currentTime);

        display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.setFont(ArialMT_Plain_10);
        display.drawString(128, 0, currentDate);
      
        display.drawLine(64, 20, 64, 50);
      
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        //display.drawString(32, 13, Tomorrow_dt);
        display.drawString(0, 26, "Min:   "+Tomorrow_temp_min+"°C"); 
        display.drawString(0, 39, "Max:   "+Tomorrow_temp_max+"°C"); 
      
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        //display.drawString(96, 13, DayAftTmw_dt);
        display.drawString(66, 26, "Min:   "+DayAftTmw_temp_min+"°C"); 
        display.drawString(66, 39, "Max:   "+DayAftTmw_temp_max+"°C"); 
        

display.display();
}
