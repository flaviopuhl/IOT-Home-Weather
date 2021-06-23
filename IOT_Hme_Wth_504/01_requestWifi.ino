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
