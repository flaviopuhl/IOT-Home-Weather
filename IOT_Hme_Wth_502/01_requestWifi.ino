/***************************************************************************************
**                          Send WiFi info to serial port
***************************************************************************************/
void requestCurrentWiFi()
{
 
    Serial.println("############### Current WIFI ###############\n");
    Serial.print("SSID             : "); Serial.println(WiFi.SSID());
    Serial.print("IP               : "); Serial.println(WiFi.localIP());
    Serial.print("RSSI             : "); Serial.println(WiFi.RSSI());
    Serial.print("Status           : "); Serial.println(wl_status_to_string(WiFi.status()));
    Serial.println();

      String wifiSSID = WiFi.SSID();
      String wifiIP = ipToString(WiFi.localIP());
      String WifiRSSI = String(WiFi.RSSI());

        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
          display.drawString(0, 0,  "ID..."); display.drawString(30, 0,  wifiSSID);
          display.drawString(0, 13, "IP..."); display.drawString(30, 13, wifiIP);
          display.drawString(0, 26, "RSSI."); display.drawString(30, 26, WifiRSSI);
          display.drawString(0, 39, "STS.."); display.drawString(30, 39, wl_status_to_string(WiFi.status()));
          
          
          display.display();
    
}

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}

String ipToString(IPAddress ip){
          String s="";
          for (int i=0; i<4; i++)
            s += i  ? "." + String(ip[i]) : String(ip[i]);
          return s;
        }
