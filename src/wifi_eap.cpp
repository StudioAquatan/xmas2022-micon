#include "wifi_eap.h"

#include <Arduino.h>
#include <time.h>

#include "secrets/wifi_configs.h"

static struct tm timeInfo;
static const char NTP_PRIMARY[] = "ntp.nict.jp";
static const char NTP_SECONDARY[] = "ntp.jst.mfeed.ad.jp";

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    Serial.println("Disconnected from WiFi access point");
    Serial.print("WiFi lost connection. Reason: ");
    Serial.println(info.wifi_sta_disconnected.reason);
    Serial.println("Trying to Reconnect");
    if (strcmp(WIFI_SSID, "eduroam") == 0) {
        WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
    } else {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
}

void setupWiFi() {
    Serial.println("Starting wifi");
    Serial.print("Connecting to network: ");
    Serial.println(WIFI_SSID);

    WiFi.disconnect(true);
    WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    WiFi.mode(WIFI_STA);
    // WiFi.setSleep(false); // May help with disconnect? Seems to have been removed from WiFi

    if (strcmp(WIFI_SSID, "eduroam") == 0) {
        WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
    } else {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }

    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    configTime(9 * 3600L, 0, NTP_PRIMARY, NTP_SECONDARY);
    Serial.println("Waiting on time sync...");
    while (time(nullptr) < 1510644967) {
        delay(10);
    }
    getLocalTime(&timeInfo);
    Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}
