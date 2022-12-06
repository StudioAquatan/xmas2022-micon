#ifndef INCLUDE_GUARD_AWS_MQTT_H
#define INCLUDE_GUARD_AWS_MQTT_H

#include <MQTTClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

extern MQTTClient mqttClient;

extern void setupAWSIoT();
extern void publishCurrentPatternNumber();
extern void updateState(String &payload);
extern void messageHandler(String &topic, String &payload);

extern volatile uint8_t gCurrentPatternNumber;

#endif  // INCLUDE_GUARD_AWS_MQTT_H
