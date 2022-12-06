#include "aws_mqtt.h"

#include <ArduinoJson.h>

#include "aws_s3_ota.h"
#include "secrets/aws.h"
#include "secrets/wifi_configs.h"

WiFiClientSecure wifiClientMQTT = WiFiClientSecure();
MQTTClient mqttClient = MQTTClient(256);

void connectAWS() {
    // Configure WiFiClientSecure to use the AWS IoT device credentials
    wifiClientMQTT.setCACert(AWS_CERT_CA);
    wifiClientMQTT.setCertificate(AWS_CERT_CRT);
    wifiClientMQTT.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    mqttClient.begin(awsIoTInfo.endpoint, awsIoTInfo.port, wifiClientMQTT);

    // Create a message handler
    mqttClient.onMessage(messageHandler);

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
    mqttClient.subscribe(awsIoTSubscribeTopic);
    mqttClient.subscribe(otaURLSubscribeTopic);

    Serial.println("AWS IoT Connected!");
}

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

    mqttClient.publish(awsIoTPublishTopic, jsonBuffer);
}

void updateState(String &payload) {
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

void messageHandler(String &topic, String &payload) {
    Serial.println("incoming: " + topic + " - " + payload);
    if (topic == otaURLSubscribeTopic) {
        execOTA(payload);
    } else if (topic == awsIoTSubscribeTopic) {
        updateState(payload);
    }
}
