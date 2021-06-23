void updateScreensGeo(){

        display.clear(); // clear the display
 
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);  


          display.drawString(0, 0,  "Geolocation"); 
          display.drawString(0, 13, "City:"); display.drawString(30, 13, city);
          display.drawString(0, 26, "Lat :"); display.drawString(30, 26, String(lat));
          display.drawString(0, 39, "Lon :"); display.drawString(30, 39, String(lon));
        
          display.display();
}
