#ifndef INCLUDE_GUARD_AWS_H
#define INCLUDE_GUARD_AWS_H

extern char deviceShadowPublishTopic[128];
extern char deviceShadowSubscribeTopic[128];
extern char heatbeatPublishTopic[128];
extern char otaURLSubscribeTopic[128];

struct host_info {
    const char *endpoint;
    const int port;
};

extern const host_info awsIoTInfo;
extern const host_info awsS3Info;

extern const char THINGNAME[];
extern const char AWS_CERT_CA[];
extern const char AWS_CERT_CRT[];
extern const char AWS_CERT_PRIVATE[];

#endif  // INCLUDE_GUARD_AWS_CONFIG_H
