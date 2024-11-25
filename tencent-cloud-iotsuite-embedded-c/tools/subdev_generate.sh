#!/bin/bash

python tc_iot_code_generator.py -c ../examples/advanced_edition/sub_device/iot-product.json code_templates/sub_device/app_main.c code_templates/sub_device/tc_iot_device_*
python tc_iot_code_generator.py -s subdev01 -c ../examples/advanced_edition/sub_device/iot-product-subdev01.json code_templates/sub_device/tc_iot_sub_device_logic.*

# 2 to n sub devicecs

si=02
mv ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.c ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic_all.c
head -n -1 ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.h > ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic_all.h

python tc_iot_code_generator.py -s subdev${si} -c ../examples/advanced_edition/sub_device/iot-product-subdev${si}.json code_templates/sub_device/tc_iot_sub_device_logic.*

sed -i '1d;2d;3d;4d' ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.c
cat ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.c >> ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic_all.c
sed -i '1d;2d;3d;4d;5d;6d' ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.h
cat ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.h >> ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic_all.h

mv ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic_all.c ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.c
mv ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic_all.h ../examples/advanced_edition/sub_device/tc_iot_sub_device_logic.h

# si=03
# ...
# si=n

