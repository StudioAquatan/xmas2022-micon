PIO=platformio
AWS_S3_BUCKET=xmas2022-esp32-ota
ENVS=ESP32_MAIN_GATE ESP32_CAFETERIA_GATE ESP32_CAFETERIA_TREE

.PHONY: all
all: $(ENVS)

.PHONY: versioning
versioning:
	python versioning.py

$(ENVS):
	$(PIO) run --environment $@
	aws s3 cp .pio/build/$@/firmware.bin s3://$(AWS_S3_BUCKET)/$@/
	aws iot-data publish --topic "$@/OTA/request" --cli-binary-format raw-in-base64-out --payload '{"state": "Request"}'

.PHONY: s3-clean
s3-clean:
	aws s3 rm s3://$(AWS_S3_BUCKET)/ --recursive
