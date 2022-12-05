
#ifndef INCLUDE_GUARD_WIFI_EAP_H
#define INCLUDE_GUARD_WIFI_EAP_H

#include <WiFi.h>

extern void setupWiFi();
extern void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
extern void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
extern void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

#endif  // INCLUDE_GUARD_WIFI_EAP_H
