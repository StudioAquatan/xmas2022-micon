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

void mqtt_main_loop(void *arg);
void led_main_loop(void *arg);

void setup() {
    delay(3000);
    Serial.begin(9600);

    sprintf(deviceShadowPublishTopic, "$aws/things/%s/shadow/update", THINGNAME);
    sprintf(deviceShadowSubscribeTopic, "$aws/things/%s/shadow/update/delta", THINGNAME);
    sprintf(heatbeatPublishTopic, "%s/heatbeat", THINGNAME);
    sprintf(otaURLSubscribeTopic, "%s/OTA", THINGNAME);
    sprintf(binaryPath, "/%s/firmware.bin", THINGNAME);
    Serial.printf("pubTopicShadow=%s\n", deviceShadowPublishTopic);
    Serial.printf("subTopicShadow=%s\n", deviceShadowSubscribeTopic);
    Serial.printf("subTopicOTA=%s\n", otaURLSubscribeTopic);

    setupWiFi();
    setupAWSIoT();
    publishCurrentPatternNumber();

    xTaskCreatePinnedToCore(
        mqtt_main_loop,
        "mqtt_main_loop",
        8192,
        NULL,
        1,
        NULL,
        0);

    xTaskCreatePinnedToCore(
        led_main_loop,
        "led_main_loop",
        8192,
        NULL,
        2,
        NULL,
        1);
}

void loop() {
    delay(1000);
}

// void loop() {
void mqtt_main_loop(void *arg) {
    while (1) {
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
                publishHeatbeat();
            }
        }

        delay(1000);
    }
}

void led_main_loop(void *arg) {
    while (1) {
        Serial.println("led_main_loop");
        delay(1000);
    }
}
