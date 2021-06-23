void updateScreensWifi(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  
      String wifiSSID = WiFi.SSID();
      String wifiIP = ipToString(WiFi.localIP());
      String WifiRSSI = String(WiFi.RSSI());

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 0, "Wifi Status");

  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0, 13, "ID..."); display->drawString(30, 13, wifiSSID);
  display->drawString(0, 26, "IP..."); display->drawString(30, 26, wifiIP);
  display->drawString(0, 39, "RSSI."); display->drawString(30, 39, WifiRSSI);
  //display->drawString(0, 52, "STS.."); display->drawString(30, 52, wl_status_to_string(WiFi.status()));

}

String ipToString(IPAddress ip){
          String s="";
          for (int i=0; i<4; i++)
            s += i  ? "." + String(ip[i]) : String(ip[i]);
          return s;
}
        
