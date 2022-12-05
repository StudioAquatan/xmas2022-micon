#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "aws_mqtt.h"
#include "aws_s3_ota.h"
#include "secrets/aws.h"
#include "secrets/wifi_configs.h"

volatile uint8_t gCurrentPatternNumber = 0;

void setup() {
    Serial.begin(9600);

    sprintf(awsIoTPublishTopic, "$aws/things/%s/shadow/update", THINGNAME);
    sprintf(awsIoTSubscribeTopic, "$aws/things/%s/shadow/update/delta", THINGNAME);
    sprintf(otaURLSubscribeTopic, "%s/OTA", THINGNAME);
    sprintf(binaryPath, "/%s/firmware.bin", THINGNAME);
    Serial.printf("pubTopicShadow=%s\n", awsIoTPublishTopic);
    Serial.printf("subTopicShadow=%s\n", awsIoTSubscribeTopic);
    Serial.printf("subTopicOTA=%s\n", otaURLSubscribeTopic);

    connectAWS();
    publishCurrentPatternNumber();
}

void loop() {
    mqttClient.loop();
    delay(1000);
}
