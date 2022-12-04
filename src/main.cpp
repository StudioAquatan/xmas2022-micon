#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <Update.h>
#include <WiFiClientSecure.h>

#include "WiFi.h"
#include "secrets.h"

// The MQTT topics that this device should publish/subscribe
char awsIoTPublishTopic[128];
char awsIoTSubscribeTopic[128];
char otaURLSubscribeTopic[128];

WiFiClientSecure wifiClientMQTT = WiFiClientSecure();
WiFiClientSecure wifiClientOTA = WiFiClientSecure();
MQTTClient mqttClient = MQTTClient(256);

volatile uint8_t gCurrentPatternNumber = 0;

// S3 Bucket Config
int awsS3Port = 443;
char binaryPath[128];

// Variables to validate
// response from S3
long contentLength = 0;
bool isValidContentType = false;

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

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
    return header.substring(strlen(headerName.c_str()));
}

void execOTA(String &payload) {
    Serial.println("Connecting to: " + String(AWS_S3_ENDPOINT));
    wifiClientOTA.setCACert(AWS_CERT_CA);
    // Connect to S3
    if (wifiClientOTA.connect(AWS_S3_ENDPOINT, awsS3Port)) {
        // Connection Succeed.
        // Fecthing the bin
        Serial.println("Fetching Bin: " + String(binaryPath));

        // Get the contents of the bin file
        wifiClientOTA.print(String("GET ") + String(binaryPath) + " HTTP/1.1\r\n" +
                            "Host: " + String(AWS_S3_ENDPOINT) + "\r\n" +
                            "Cache-Control: no-cache\r\n" +
                            "Connection: close\r\n\r\n");

        // Check what is being sent
        //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
        //                 "Host: " + host + "\r\n" +
        //                 "Cache-Control: no-cache\r\n" +
        //                 "Connection: close\r\n\r\n");

        unsigned long timeout = millis();
        while (wifiClientOTA.available() == 0) {
            if (millis() - timeout > 5000) {
                Serial.println("Client Timeout !");
                wifiClientOTA.stop();
                return;
            }
        }
        // Once the response is available,
        // check stuff

        /*
           Response Structure
            HTTP/1.1 200 OK
            x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
            x-amz-request-id: 2D56B47560B764EC
            Date: Wed, 14 Jun 2017 03:33:59 GMT
            Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
            ETag: "d2afebbaaebc38cd669ce36727152af9"
            Accept-Ranges: bytes
            Content-Type: application/octet-stream
            Content-Length: 357280
            Server: AmazonS3

            {{BIN FILE CONTENTS}}

        */
        while (wifiClientOTA.available()) {
            // read line till /n
            String line = wifiClientOTA.readStringUntil('\n');
            // remove space, to check if the line is end of headers
            line.trim();

            // if the the line is empty,
            // this is end of headers
            // break the while and feed the
            // remaining `client` to the
            // Update.writeStream();
            if (!line.length()) {
                // headers ended
                break;  // and get the OTA started
            }

            // Check if the HTTP Response is 200
            // else break and Exit Update
            if (line.startsWith("HTTP/1.1")) {
                if (line.indexOf("200") < 0) {
                    Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
                    Serial.println();
                    break;
                }
            }

            // extract headers here
            // Start with content length
            if (line.startsWith("Content-Length: ")) {
                contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
                Serial.println("Got " + String(contentLength) + " bytes from server");
            }

            // Next, the content type
            if (line.startsWith("Content-Type: ")) {
                String contentType = getHeaderValue(line, "Content-Type: ");
                Serial.println("Got " + contentType + " payload.");
                if (contentType == "application/octet-stream") {
                    isValidContentType = true;
                }
            }
        }
    } else {
        // Connect to S3 failed
        // May be try?
        // Probably a choppy network?
        Serial.println("Connection to " + String(AWS_S3_ENDPOINT) + " failed. Please check your setup");
        // retry??
        // execOTA();
    }

    // Check what is the contentLength and if content type is `application/octet-stream`
    Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

    // check contentLength and content type
    if (contentLength && isValidContentType) {
        // Check if there is enough to OTA Update
        bool canBegin = Update.begin(contentLength);

        // If yes, begin
        if (canBegin) {
            Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
            // No activity would appear on the Serial monitor
            // So be patient. This may take 2 - 5mins to complete
            size_t written = Update.writeStream(wifiClientOTA);

            if (written == contentLength) {
                Serial.println("Written : " + String(written) + " successfully");
            } else {
                Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                // retry??
                // execOTA();
            }

            if (Update.end()) {
                Serial.println("OTA done!");
                if (Update.isFinished()) {
                    Serial.println("Update successfully completed. Rebooting.");
                    ESP.restart();
                } else {
                    Serial.println("Update not finished? Something went wrong!");
                }
            } else {
                Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            }
        } else {
            // not enough space to begin OTA
            // Understand the partitions and
            // space availability
            Serial.println("Not enough space to begin OTA");
            wifiClientOTA.flush();
        }
    } else {
        Serial.println("There was no content in the response");
        wifiClientOTA.flush();
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
