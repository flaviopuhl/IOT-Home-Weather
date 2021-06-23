void updateScreensWifi(){
  
      String wifiSSID = WiFi.SSID();
      String wifiIP = ipToString(WiFi.localIP());
      String WifiRSSI = String(WiFi.RSSI());

        display.clear(); // clear the display
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
          display.drawString(0, 0,  "Network"); 
          display.drawString(0, 13, "ID..."); display.drawString(30, 13, wifiSSID);
          display.drawString(0, 26, "IP..."); display.drawString(30, 26, wifiIP);
          display.drawString(0, 39, "RSSI."); display.drawString(30, 39, WifiRSSI);
          //display.drawString(0, 52, "STS.."); display.drawString(30, 52, wl_status_to_string(WiFi.status()));
          
          display.display();
}

String ipToString(IPAddress ip){
          String s="";
          for (int i=0; i<4; i++)
            s += i  ? "." + String(ip[i]) : String(ip[i]);
          return s;
        }
