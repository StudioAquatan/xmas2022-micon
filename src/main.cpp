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
char aws_iot_publish_topic[128];
char aws_iot_subscribe_topic[128];

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void publishMessage() {
    StaticJsonDocument<200> doc;
    JsonObject root = doc.to<JsonObject>();
    JsonObject state = root.createNestedObject("state");
    // JsonObject desired = state.createNestedObject("desired");
    // desired["pattern"] = 100;
    JsonObject reported = state.createNestedObject("reported");
    reported["pattern"] = 200;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);  // print to client

    client.publish(aws_iot_publish_topic, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
    Serial.println("incoming: " + topic + " - " + payload);

    //  StaticJsonDocument<200> doc;
    //  deserializeJson(doc, payload);
    //  const char* message = doc["message"];
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
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);

    // Create a message handler
    client.onMessage(messageHandler);

    Serial.print("Connecting to AWS IOT");

    while (!client.connect(THINGNAME)) {
        Serial.print(".");
        delay(100);
    }

    if (!client.connected()) {
        Serial.println("AWS IoT Timeout!");
        return;
    }

    // Subscribe to a topic
    client.subscribe(aws_iot_subscribe_topic);

    Serial.println("AWS IoT Connected!");
}

void setup() {
    Serial.begin(9600);

    sprintf(aws_iot_publish_topic, "$aws/things/%s/shadow/update", THINGNAME);
    sprintf(aws_iot_subscribe_topic, "$aws/things/%s/shadow/update/delta", THINGNAME);
    Serial.printf("pubTopicShadow=%s\n", aws_iot_publish_topic);
    Serial.printf("subTopicShadow=%s\n", aws_iot_subscribe_topic);

    connectAWS();
}

void loop() {
    publishMessage();
    client.loop();
    delay(1000);
}
