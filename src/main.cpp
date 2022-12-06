#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "aws_mqtt.h"
#include "aws_s3_ota.h"
#include "secrets/aws.h"
#include "secrets/wifi_configs.h"
#include "wifi_eap.h"

unsigned long lastMillis = 0;
volatile uint8_t gCurrentPatternNumber = 0;

void setup() {
    Serial.begin(9600);

    sprintf(deviceShadowPublishTopic, "$aws/things/%s/shadow/update", THINGNAME);
    sprintf(deviceShadowSubscribeTopic, "$aws/things/%s/shadow/update/delta", THINGNAME);
    sprintf(otaURLSubscribeTopic, "%s/OTA", THINGNAME);
    sprintf(binaryPath, "/%s/firmware.bin", THINGNAME);
    Serial.printf("pubTopicShadow=%s\n", deviceShadowPublishTopic);
    Serial.printf("subTopicShadow=%s\n", deviceShadowSubscribeTopic);
    Serial.printf("subTopicOTA=%s\n", otaURLSubscribeTopic);

    setupWiFi();
    setupAWSIoT();
    publishCurrentPatternNumber();
}

void loop() {
    mqttClient.loop();

    // WiFiの再接続を待って、MQTTを接続しにいく
    if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
        while (!mqttClient.connect(THINGNAME)) {
            Serial.print(".");
            delay(100);
        }
        Serial.println("");

        if (!mqttClient.connected()) {
            Serial.println("AWS IoT Timeout!");
            return;
        }
    }

    if (mqttClient.connected()) {
        // publish a heatbeat roughly every second.
        if (millis() - lastMillis > 60000) {
            lastMillis = millis();
        }
    }

    delay(1000);
}
