# xmas2022-micon

企画部(irodori)と共同したクリスマス企画で、イルミネーションの点灯アニメーションを制御するマイコンプログラムのリポジトリ。

関連レポジトリ

- [xmas-api](https://github.com/StudioAquatan/xmas-api) Twitterの監視と点灯ルール配信
- [xmas-frontend](https://github.com/StudioAquatan/xmas-frontend) ルール設定を行うためのフロントエンド
- [xmas2022-micon](https://github.com/StudioAquatan/xmas2022-micon) イルミネーション点灯制御


2021年版：[xmas2021-micon](https://github.com/StudioAquatan/xmas2021-micon)

担当：@Hiroya-W

## Dependencies

- Espressif ESP32 Dev Module
- 256dpi/MQTT@^2.5.0
- bblanchon/ArduinoJson@^6.19.4

## secretsを定義する

`lib/secrets/src/`内にexampleファイルを配置しているので、それぞれを以下のようにファイル名を変更します。

- `aws.cpp.example` -> `aws.cpp`
- `wifi_configs.cpp.example` -> `wifi_configs.cpp`

### AWS secrets

`lib/secrets/src/aws.cpp` に定義します。

#### AWS IoT Coreエンドポイント

`const host_info awsIoTInfo`に設定します。AWS IoT Coreエンドポイントについては[AWS IoT デバイスデータとサービスエンドポイント](https://docs.aws.amazon.com/ja_jp/iot/latest/developerguide/iot-connect-devices.html?icmpid=docs_iot_hp_settings#iot-connect-device-endpoints)を参考にしてください。

#### AWS S3バケットURL

OTA用のfirmwareを取得する際に利用するS3バケットへのURLを、`const host_info awsS3Info`に設定します。
AWS S3バケットへのアクセス方法については、[バケットへのアクセス方法](https://docs.aws.amazon.com/ja_jp/AmazonS3/latest/userguide/access-bucket-intro.html)を参考にしてください。

取得するfirmwareへのURLは以下のようになります。`THINGNAME`は[# モノの名前](#モノの名前)で設定した文字列となります。

- `https://<awsS3Info.endpoint>:<awsS3Info.port>/<THINGNAME>/firmware.bin`

#### AWS IoT Topic名

Topic名は設定する必要はありません。
Topic名は、`src/main.cpp`の`void setup()`関数で[# モノの名前](#モノの名前)で定義した文字列利用して、以下のように初期化されます。

- `deviceShadowPublishTopic`: `$aws/things/<THINGNAME>/shadow/update`
- `deviceShadowSubscribeTopic`: `$aws/things/<THINGNAME>/shadow/update/delta`
- `otaReplyPublishTopic`: `<THINGNAME>/OTA/reply`
- `otaRequestSubscribeTopic`: `<THINGNAME>/OTA/request`

#### モノの名前

`const char THINGNAME[]`に設定します。AWS IoT Coreで登録したモノの名前をと一致しておく必要があります。

#### モノの接続情報

接続するために、以下の3つを設定しておく必要があります。これらはモノを登録した際に取得することが出来ます。
詳しくは[AWS IoT のモノ、証明書、およびプライベートキーを作成する](https://docs.aws.amazon.com/ja_jp/iot/latest/developerguide/iot-moisture-create-thing.html)を参照してください。

- `AWS_CERT_CA`: AmazonルートCA証明書
- `AWS_CERT_CRT`: デバイス証明書
- `AWS_CERT_PRIVATE`: プライベートキーファイル

### Wi-Fi secrets

Wi-Fiの接続情報は`lib/secrets/src/wifi_configs.cpp`に定義します。

#### eduroamに接続する場合

eduroamに接続する場合は以下の5つを設定します。

- `WIFI_SSID`: `"eduroam"`
- `WIFI_PASSWORD`: `""`
- `EAP_IDENTITY`: 所属機関でのユーザID
    - 別の所属のeduroamに接続する場合は`所属機関でのユーザID@所属機関のドメイン名`とする必要があります。
- `EAP_USERNAME`: 通常`EAP_IDENTITY`と同じ
- `EAP_PASSWORD`: 所属機関で利用しているパスワード


#### eduroam以外のSSIDに接続する場合

eduroam以外に接続する場合は以下の2つを設定します。

- `WIFI_SSID`: 接続したいSSID
- `WIFI_PASSWORD`: SSIDへ接続するためのパスワード

## 点灯パターンの変更

AWS IoT CoreのDevice Shadowの機能を利用して、IoTデバイスへ通知する。

例えば、モノ`<THING_NAME>`の点灯パターン(数値)を`<PATTERN_NUMBER>`に変更する場合は、

- トピック名：`$aws/things/<THING_NAME>/shadow/update`
- payload：
    ```json
    {
        "state": { 
            "desired": {
                "patternId": <PATTERN_NUMBER>
            }
        } 
    }
    ```

にpayloadをPublishする。

```bash
aws iot-data publish --topic "\$aws/things/<THING_NAME>/shadow/update" --cli-binary-format raw-in-base64-out --payload '{"state": { "desired": { "patternId": <PATTERN_NUMBER> } } }'
```

## OTA

AWS S3にバイナリを配置し、IoTデバイスへ通知することでOTAが実行される。

1. S3にバイナリをアップロードする。

```bash
aws s3 cp .pio/build/<ENVIRONMENT>/firmware.bin s3://<BUCKET_NAME>/<THING_NAME>/
```

2. IoTデバイスへ通知する。

- トピック名：`<THING_NAME>/OTA/request`
- payload：
    ```json
    {
        "url": "https://<BUCKET_NAME>.s3.<REGION>.amazonaws.com/<BUCKET_NAME>/firmware.bin"
    }
    ```

にpayloadをPublishする。

```bash
aws iot-data publish --topic "<THING_NAME>/OTA/request" --cli-binary-format raw-in-base64-out --payload '{"url": "https://<BUCKET_NAME>.s3.<REGION>.amazonaws.com/<BUCKET_NAME>/firmware.bin"}'
```

3. IoTデバイスがリクエストを受け付けた場合、以下のメッセージを受け取る。

- トピック名：`<THING_NAME>/OTA/reply`
- payload：
    ```json
    {
        "state": "Received"
    }
    ```
