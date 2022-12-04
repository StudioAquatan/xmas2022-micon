# xmas2022-micon

企画部(irodori)と共同したクリスマス企画で、イルミネーションの点灯アニメーションを制御するマイコンプログラムのリポジトリ。

2021年版：[xmas2021-micon](https://github.com/StudioAquatan/xmas2021-micon)

担当：@Hiroya-W

## Dependencies

- Espressif ESP32 Dev Module
- 256dpi/MQTT@^2.5.0
- bblanchon/ArduinoJson@^6.19.4

## 点灯パターンの変更

AWS IoT CoreのDevice Shadowの機能を利用して、IoTデバイスへ通知する。

例えば、モノ`<THING_NAME>`の点灯パターン(数値)を`<PATTERN_NUMBER>`に変更する場合は、

- トピック名：`$aws/things/<THING_NAME>/shadow/update`
- payload：
    ```json
    {
        "state": { 
            "desired": {
                "pattern": <PATTERN_NUMBER> 
            }
        } 
    }
    ```

にpayloadをPublishする。

```bash
aws iot-data publish --topic "\$aws/things/<THING_NAME>/shadow/update" --cli-binary-format raw-in-base64-out --payload '{"state": { "desired": { "pattern": <PATTERN_NUMBER> } } }'
```

## OTA

AWS S3にバイナリを配置し、IoTデバイスへ通知することでOTAが実行される。

1. S3にバイナリをアップロードする。

```bash
aws s3 cp .pio/build/<ENVIRONMENT>/firmware.bin s3://<BUCKET_NAME>/<THING_NAME>/
```

2. IoTデバイスへ通知する。

- トピック名：`<THING_NAME>/OTA`
- payload：URLを指定出来るようにするつもりだが、現在は固定になっているので、空のpayloadで良い。
    ```json
    {}
    ```

にpayloadをPublishする。

```bash
aws iot-data publish --topic "<THING_NAME>/OTA" --cli-binary-format raw-in-base64-out --payload '{}'
```
