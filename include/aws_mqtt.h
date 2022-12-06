#ifndef INCLUDE_GUARD_AWS_MQTT_H
#define INCLUDE_GUARD_AWS_MQTT_H

#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

extern MQTTClient mqttClient;

extern void printDeserializationError(const DeserializationError &error);
extern void setupAWSIoT();
extern void publishCurrentPatternNumber();
extern void publishReplyOTARequest(const String &message);
extern void updateState(String &payload);
extern void messageHandler(String &topic, String &payload);

extern volatile uint8_t gCurrentPatternNumber;

#endif  // INCLUDE_GUARD_AWS_MQTT_H
