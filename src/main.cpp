/*
  Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify,
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <WiFiClientSecure.h>

#include "WiFi.h"
#include "secrets.h"

// The MQTT topics that this device should publish/subscribe
char AWSIoTPublishTopic[128];
char AWSIoTSubscribeTopic[128];

WiFiClientSecure wifiClientMQTT = WiFiClientSecure();
MQTTClient mqttClient = MQTTClient(256);

volatile uint8_t gCurrentPatternNumber = 0;

void publishCurrentPatternNumber() {
    /* Example of publishing a message
        {
            "state": {
                "reported": {
                    "pattern": 1
                }
            }
        }
    */
    const int capacity = 3 * JSON_OBJECT_SIZE(1);
    StaticJsonDocument<capacity> doc;

    JsonObject root = doc.to<JsonObject>();
    JsonObject state = root.createNestedObject("state");
    JsonObject reported = state.createNestedObject("reported");
    reported["pattern"] = gCurrentPatternNumber;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);  // print to client

    mqttClient.publish(AWSIoTPublishTopic, jsonBuffer);
}

void updateStateHandler(String &topic, String &payload) {
    /* Example of subscribing a message
    {
        "version":1102,
        "timestamp":1670132215,
        "state":{
            "pattern": 100
        },
        "metadata": {
            "pattern":{
                "timestamp":1670082769
            }
        }
    }
    */
    const int capacity = JSON_OBJECT_SIZE(4) + 7 * JSON_OBJECT_SIZE(1);
    StaticJsonDocument<capacity> doc;

    Serial.println("incoming: " + topic + " - " + payload);
    DeserializationError err = deserializeJson(doc, payload);

    switch (err.code()) {
        case DeserializationError::EmptyInput:
            Serial.println("DeserializationError::EmptyInput. The input was empty or contained only spaces.");
            break;
        case DeserializationError::IncompleteInput:
            Serial.println("DeserializationError::IncompleteInput. The input was valid but ended prematurely.");
            break;
        case DeserializationError::InvalidInput:
            Serial.println("DeserializationError::InvalidInput. the input was not a valid JSON document.");
            break;
        case DeserializationError::NoMemory:
            Serial.println("DeserializationError::NoMemory. The JsonDocument was too small.");
            break;
        case DeserializationError::TooDeep:
            Serial.println(
                "DeserializationError::TooDeep. The input was valid, but it contained too many "
                "nesting levels; we’ll talk about that later in the book.");
            break;
        default:
            break;
    }

    auto pattern = doc["state"]["pattern"] | -1;
    if (pattern == -1) {
        Serial.println("Payload did not contain a pattern. Skipping.");
    } else {
        gCurrentPatternNumber = uint8_t(pattern);
        Serial.println("Changed pattern to: " + String(gCurrentPatternNumber));
        publishCurrentPatternNumber();
    }
}

void connectAWS() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Configure WiFiClientSecure to use the AWS IoT device credentials
    wifiClientMQTT.setCACert(AWS_CERT_CA);
    wifiClientMQTT.setCertificate(AWS_CERT_CRT);
    wifiClientMQTT.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    mqttClient.begin(AWS_IOT_ENDPOINT, 8883, wifiClientMQTT);

    // Create a message handler
    mqttClient.onMessage(updateStateHandler);

    Serial.print("Connecting to AWS IOT");

    while (!mqttClient.connect(THINGNAME)) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("");

    if (!mqttClient.connected()) {
        Serial.println("AWS IoT Timeout!");
        return;
    }

    // Subscribe to a topic
    mqttClient.subscribe(AWSIoTSubscribeTopic);

    Serial.println("AWS IoT Connected!");
}

void setup() {
    Serial.begin(9600);

    sprintf(AWSIoTPublishTopic, "$aws/things/%s/shadow/update", THINGNAME);
    sprintf(AWSIoTSubscribeTopic, "$aws/things/%s/shadow/update/delta", THINGNAME);
    Serial.printf("pubTopicShadow=%s\n", AWSIoTPublishTopic);
    Serial.printf("subTopicShadow=%s\n", AWSIoTSubscribeTopic);

    connectAWS();
    publishCurrentPatternNumber();
}

void loop() {
    mqttClient.loop();
    delay(1000);
}
