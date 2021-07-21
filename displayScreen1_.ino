void Screen_Actual(){

display.clear();

        display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0, "Atual");
        display.drawString(0, 13, "EXTERNO");
        display.drawString(90, 13, "FEELS");
      
      display.setFont(Dialog_plain_30);           // Show ext temp
        display.drawString(0, 31, Current_temp);
      
      display.setFont(Dialog_plain_16);
        display.drawString(68, 43, "°C");                         

      display.setTextAlignment(TEXT_ALIGN_RIGHT);
        display.setFont(ArialMT_Plain_10);
        display.drawString(128, 0, currentDate);
         display.setFont(Dialog_plain_16);
        display.drawString(128, 25, Current_feels_like+ " ºC");
      
display.display();

}
