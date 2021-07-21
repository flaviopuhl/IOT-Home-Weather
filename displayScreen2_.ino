void Screen_TempInt(){

display.clear();

        display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0, currentTime);

        display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.setFont(ArialMT_Plain_10);
        display.drawString(128, 0, currentDate);

        display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
        display.drawString(0, 13, "INTERNO");

       display.setFont(Dialog_plain_40);
        display.drawString(10, 25, string_tempC);
       display.setFont(Dialog_plain_16);
        display.drawString(100, 45, "°C");      
      
display.display();
}
