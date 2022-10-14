# RAKwireless WisBlock
| <img src="assets/repo/RAK.png" alt="Modules" width="150"> | <img src="assets/repo/rakstar.jpg" alt="Modules" width="100"> | [![Build Status](https://github.com/RAKWireless/WisBlock/workflows/RAK%20BSP%20Arduino%20Build%20CI/badge.svg)](https://github.com/RAKWireless/WisBlock/actions) |    
| :-: | :-: | :-: |
| <img src="assets/repo/rak11200.png" alt="Modules" width="250"> | <img src="assets/repo/RAK4631.png" alt="Modules" width="250"> | <img src="assets/repo/RAK11310.png" alt="Modules" width="250"> |    

## Introduction
WisBlock is an amazing product built by the RAK company for IoT industry. It can build circuits like clicking blocks together to quickly realize your idea.

WisBlock consists of WisBlock Base, WisBlock Core, WisBlock Sensor and WisBlock IO.
- WisBlock Base → the block that the whole system is build on.    
- WisBlock Core → the processing unit with the micro-controller.    
- WisBlock Sensor → the blocks with variety of sensors.    
- WisBlock IO → the blocks that extend the communication options and interfacing capabilities of the WisBlock.

![WisBlock-Assembly](assets/repo/WisBlock-Assembly.png)

This repository is created to help you start with WisBlock as simple as possible. Let's get started.

**RAKwireless invests time and resources providing this open source code, please support RAKwireless and open-source hardware by purchasing products from RAKwireless!**

For support and questions about RAKwireless products please visit our [forum](https://forum.rakwireless.com/)

To buy WisBlock modules please visit our [online store](https://store.rakwireless.com/)

----
## Where to find what

----

### Documentation

- Documentation  
    - [Quick Start Guide RAK4630](https://docs.rakwireless.com/Product-Categories/WisBlock/RAK4631/Quickstart/)
    - [Quick Start Guide RAK11200](https://docs.rakwireless.com/Product-Categories/WisBlock/RAK11200/Quickstart/)
    - [Quick Start Guide RAK11300](https://docs.rakwireless.com/Product-Categories/WisBlock/RAK11310/Quickstart/)
    - [Quick Start Guide WisBlock Base](quickstart/Base)
    - [Quick Start Guide WisBlock IO Modules](quickstart/IOModules)
    - [Quick Start Guide WisBlock Sensor Modules](quickstart/SensorModules)
----

### Examples

- [Examples](/examples/)    
    - [Common for all WisBlock Cores](/examples/common/)
        - [Slot A - D Modules](/examples/common/sensors/)
            - [RAK1901_Temperature&Humidity_SHTC3](/examples/common/sensors/RAK1901_Temperature_Humidity_SHTC3/)
            - [RAK1902_Pressure_LPS22HB](/examples/common/sensors/RAK1902_Pressure_LPS22HB/)
            - [RAK1903_Optical_OPT3001](/examples/common/sensors/RAK1903_Optical_OPT3001/)
            - [RAK1904_Accelerate_LIS3DH](/examples/common/sensors/RAK1904_Accelerate_LIS3DH/)
            - [RAK1905_9_Axis_MPU9250](/examples/common/sensors/RAK1905_9_Axis_MPU9250/)
            - [RAK1906_Environment_BME680](/examples/common/sensors/RAK1906_Environment_BME680/)
            - [RAK1910_GPS_UBLOX7](/examples/common/sensors/RAK1910_GPS_UBLOX7/)
            - [RAK1920_MikroBUS_Temperature_TMP102](/examples/common/sensors/RAK1920_MikroBUS_Temperature_TMP102/)
            - [RAK1920_QWIIC_AirQuality_SGP30](/examples/common/sensors/RAK1920_QWIIC_AirQuality_SGP30/)
            - [RAK1920_Grove_PIR_AS312](/examples/common/sensors/RAK1920_Grove_PIR_AS312/)
			- [RAK12002_RTC_DateTime_RV-3028-C7](/examples/common/sensors/RAK12002_RTC_DateTime_RV-3028-C7)
			- [RAK12002_RTC_EEPROM_RV-3028-C7](/examples/common/RAK12002_RTC_EEPROM_RV-3028-C7)
            - [RAK12003_FIR_MLX90632](/examples/common/sensors/RAK12003_FIR_MLX90632/)
            - [RAK12010_VEML7700_Light](/examples/common/sensors/RAK12010_VEML7700_Light/)
            - [RAK12011_MEMS_Pressure_LPS33HW](/examples/common/sensors/RAK12011_MEMS_Pressure_LPS33HW/)
            - [RAK12019_UVlight_LTR390](/examples/common/sensors/RAK12019_UVlight_LTR390/)
            - [RAK12025_Gyroscope](/examples/common/sensors/RAK12025_Gyroscope/)
            - [RAK12027_Earthquake](/examples/common/sensors/RAK12027_Earthquake/)
            - [RAK12032_3_Axis_ADXL313](/examples/common/sensors/RAK12032_3_Axis_ADXL313/)
            - [RAK12033_6_Axis_IIM42652](/examples/common/sensors/RAK12033_6_Axis_IIM42652/)
            - [RAK12034_9_Axis_BMX160](/examples/common/sensors/RAK12034_9_Axis_BMX160/)
            - [RAK12047_VOC_GetVOCIndex_SGP40](/examples/common/IO/RAK12047_VOC_GetVOCIndex_SGP40/)
            - [RAK12500_GPS_ZOE-M8Q](/examples/common/sensors/RAK12500_GPS_ZOE-M8Q/)
            - [RAK14008_Gesture_PAJ7620](/examples/common/sensors/RAK14008_Gesture_PAJ7620/)     
            - [RAK15000_EEPROM_AT24C02](/examples/common/sensors/RAK15000_EEPROM_AT24C02/)     
            - [RAK15001_Flash_GD25Q16C](/examples/common/sensors/RAK15001_Flash_GD25Q16C/)     
            - [RAK19002_Boost_TPS61046](/examples/common/sensors/RAK19002_Boost_TPS61046/)     
        - [IO Slot Modules](/examples/IO/)
            - [RAK1921_OLED_SSD1306](/examples/common/IO/RAK1921_OLED_SSD1306/)
            - [RAK1921_Jumping_Ball_SSD1306](/examples/common/IO/RAK1921_Jumping_Ball_SSD1306/)
            - [RAK1921_Moving_Logo_SSD1306](/examples/common/IO/RAK1921_Moving_Logo_SSD1306/)
			- [RAK12001_FingerPrint](/examples/common/IO/RAK12001_FingerPrint)
			- [RAK12004_MQ2_read](/examples/common/IO/RAK12004_MQ2_read)
			- [RAK12005_WaterDetector](/examples/common/IO/RAK12005_WaterDetector)
			- [RAK12006_MotionDetection](/examples/common/IO/RAK12006_MotionDetection)
			- [RAK12007_OLED_Show_Distance](/examples/common/IO/RAK12007_OLED_Show_Distance)
			- [RAK12008_CO2_Detection](/examples/common/IO/RAK12008_CO2_Detection)
			- [RAK12009_MQ3_AlcoholDetection](/examples/common/IO/RAK12009_MQ3_AlcoholDetection)
			- [RAK12012_HeartRate](/examples/common/IO/RAK12012_HeartRate)
			- [RAK12013_Radar_3GHZ](/examples/common/IO/RAK12013_Radar_3GHZ)
			- [RAK12014_VL53L0X_Laser_Radar](/examples/common/IO/RAK12014_VL53L0X_Laser_Radar)
			- [RAK12015_Shock_801S](/examples/common/IO/RAK12015_Shock_801S)
			- [RAK12016_Flex_FSL0095103ST](/examples/common/IO/RAK12016_Flex_FSL0095103ST)
			- [RAK12017_IR_ITR20001T](/examples/common/IO/RAK12017_IR_ITR20001T)
			- [RAK12018_CodeReader_LV3296](/examples/common/IO/RAK12018_CodeReader_LV3296)
			- [RAK12029-LDC1614](/examples/common/IO/RAK12029-LDC1614)
			- [RAK12031_T_Fork_SX1041](/examples/common/IO/RAK12031_T_Fork_SX1041)
			- [RAK12035_SoilMoisture](/examples/common/IO/RAK12035_SoilMoisture)
			- [RAK12037_CO2_SCD30](/examples/common/sensors/RAK12037_CO2_SCD30)
			- [RAK12035_SoilMoisture](/examples/common/IO/RAK12035_SoilMoisture)
			- [RAK12039_Dust_Detection](/examples/common/IO/RAK12039_Dust_Detection)
			- [RAK13001_Relay_OUT_Optocoupled_IN](/examples/common/IO/RAK13001_Relay_OUT_Optocoupled_IN)
			- [RAK13003_GPIO_Expander_Interrupt_MCP32](/examples/common/IO/RAK13003_GPIO_Expander_Interrupt_MCP32)
			- [RAK13003_GPIO_Expander_IO_MCP32](/examples/common/IO/RAK13003_GPIO_Expander_IO_MCP32)
			- [RAK13003_GPIO_Expander_Polls_MCP32](/examples/common/IO/RAK13003_GPIO_Expander_Polls_MCP32)
			- [RAK13004_PWM_Expander_PCA9685](/examples/common/IO/RAK13004_PWM_Expander_PCA9685)
			- [RAK13005_LIN_BUS](/examples/common/IO/RAK13005_LIN_BUS)
			- [RAK13006_CAN_BUS](/examples/common/IO/RAK13006_CAN_BUS)
			- [RAK13007_Relay_G5LE-14-DC3](/examples/common/IO/RAK13007_Relay_G5LE-14-DC3)
			- [RAK13010_SDI_12_BUS](/examples/common/IO/RAK13010_SDI_12_BUS)
			- [RAK13600_PN530_NFC](/examples/common/IO/RAK13600_PN530_NFC)
            - [RAK14000-Epaper-Monochrome](/examples/common/IO/RAK14000-Epaper-Monochrome/)
            - [RAK14000-Epaper-TriColor](/examples/common/IO/RAK14000-Epaper-Monochrome/)
			- [RAK14001_NCP5623_GradualDimming](/examples/common/IO/RAK14001_NCP5623_GradualDimming)
			- [RAK14001_NCP5623_RGBCycle](/examples/common/IO/RAK14001_NCP5623_RGBCycle)
			- [RAK14002_CAP1293_TouchKey](/examples/common/IO/RAK14002_CAP1293_TouchKey)
			- [RAK14003_LED_BAR_MCP32](/examples/common/IO/RAK14003_LED_BAR_MCP32)
			- [RAK14004_Atmega328pb_KeyPad](/examples/common/IO/RAK14004_Atmega328pb_KeyPad)
			- [RAK14006_Rotary_Encoder](/examples/common/IO/RAK14006_Rotary_Encoder)
			- [RAK14007_ATTINY441_Joystick](/examples/common/IO/RAK14007_ATTINY441_Joystick)
			- [RAK14012_RGB_Matrix](/examples/common/IO/RAK14012_RGB_Matrix)
			- [RAK16000_DcCurrent_INA219](/examples/common/IO/RAK16000_DcCurrent_INA219)
			- [RAK16001_ADC_ADS7830](/examples/common/IO/RAK16001_ADC_ADS7830)
			- [RAK16002_Coulomb](/examples/common/IO/RAK16002_Coulomb)
        - [Communications](/examples/common/communications/)
            - [Cellular](/examples/common/communications/Cellular/)
                - [RAK5860](/examples/common/communications/Cellular/RAK5860_BG77_Module/)
                   - [BG77_Unvarnished_Transmission](/examples/common/communications/Cellular/RAK5860_BG77_Module/BG77_Unvarnished_Transmission)
                   - [Cellular_Ping](/examples/common/communications/Cellular/RAK5860_BG77_Module/Cellular_Ping)
                   - [Hologram_Tcp](/examples/common/communications/Cellular/RAK5860_BG77_Module/Hologram_Tcp)
                   - [Hologram_Ping_LTE](/examples/common/communications/Cellular/RAK5860_BG77_Module/Hologram_Ping_LTE)
                - [RAK13101](/examples/common/communications/Cellular/RAK13101_MC20_Module/)
                   - [GPS](/examples/common/communications/Cellular/RAK13101_MC20_Module/GPS)
                   - [GSM_TCP](/examples/common/communications/Cellular/RAK13101_MC20_Module/GSM_TCP)
                   - [Unvarnished_Transmission](/examples/common/communications/Cellular/RAK13101_MC20_Module/Unvarnished_Transmission)
            - [Ethernet](/examples/common/communications/Ethernet/)
                   - [DHCP](/examples/common/communications/Ethernet/RAK13800_Ethernet_DHCP_W5100S)
                   - [HTTP CLient](/examples/common/communications/Ethernet/RAK13800_Ethernet_HTTP_Client_W5100S)
                   - [HTTP Server](/examples/common/communications/Ethernet/RAK13800_Ethernet_HTTP_Server_W5100S)
                   - [MQTT Publish](/examples/common/communications/Ethernet/RAK13800_Ethernet_MQTT_Publish_W5100S)
                   - [MQTT Subscribe](/examples/common/communications/Ethernet/RAK13800_Ethernet_MQTT_Subscribe_W5100S)
                   - [TCP Client](/examples/common/communications/Ethernet/RAK13800_Ethernet_TCP_Client_W5100S)
                   - [TCP Server](/examples/common/communications/Ethernet/RAK13800_Ethernet_TCP_Server_W5100S)
                   - [UDP](/examples/common/communications/Ethernet/RAK13800_Ethernet_UDP_W5100S)
            - [UWB](/examples/common/communications/UWB/)
                - [RAK13801](/examples/common/communications/UWB/RAK13801_UWB_module/)
                   - [BasicConnectivityTest](/examples/common/communications/UWB/RAK13801_UWB_module/BasicConnectivityTest)
                   - [BasicReceiver](/examples/common/communications/UWB/RAK13801_UWB_module/BasicReceiver)
                   - [BasicSender](/examples/common/communications/UWB/RAK13801_UWB_module/BasicSender)
                   - [SimpleAntennaCalibration](/examples/common/communications/UWB/RAK13801_UWB_module/SimpleAntennaCalibration)
                   - [StandardRTLSAnchorB_TWR](/examples/common/communications/UWB/RAK13801_UWB_module/StandardRTLSAnchorB_TWR)
                   - [StandardRTLSAnchorC_TWR](/examples/common/communications/UWB/RAK13801_UWB_module/StandardRTLSAnchorC_TWR)
                   - [StandardRTLSAnchorMain_TWR](/examples/common/communications/UWB/RAK13801_UWB_module/StandardRTLSAnchorMain_TWR)
                   - [StandardRTLSTag_TWR](/examples/common/communications/UWB/RAK13801_UWB_module/StandardRTLSTag_TWR)
                   - [StandardRTLSTag_TWR_1](/examples/common/communications/UWB/RAK13801_UWB_module/StandardRTLSTag_TWR_1)
                   - [TwoWayRangingInitiator](/examples/common/communications/UWB/RAK13801_UWB_module/TwoWayRangingInitiator)
                   - [TwoWayRangingResponder](/examples/common/communications/UWB/RAK13801_UWB_module/TwoWayRangingResponder)
    
----
    
-    
    - [RAK4630 Examples](/examples/RAK4630/) 
        - [RAK4630 Slot A - D Modules](/examples/RAK4630/sensors/)
            - [RAK18001_Buzzer](/examples/RAK4630/sensors/RAK18001_Buzzer/)
        - [RAK4630 IO Slot Modules](/examples/RAK4630/IO/) 
            - [RAK5801_4-20mA](/examples/RAK4630/IO/RAK5801_4-20mA/)
            - [RAK5802_RS485](/examples/RAK4630/IO/RAK5802_RS485/)
            - [RAK5811_0-5V](/examples/RAK4630/IO/RAK5811_0-5V/)    
			- [RAK15002_SD_Card](/examples/RAK4630/IO/RAK15002_SD_Card)
			- [RAK17000_Motor_Driver_DRV8833](/examples/RAK4630/IO/RAK17000_Motor_Driver_DRV8833)
			- [RAK17000_Stepper_Driver_DRV8833](/examples/RAK4630/IO/RAK17000_Stepper_Driver_DRV8833)
            - [RAK18000_Stereo](https://github.com/RAKWireless/RAKwireless-Audio-library/tree/main/examples)
			- [RAK19006_WirelessCharger](/examples/RAK4630/IO/RAK19006_WirelessCharger)
        - [RAK4630 Communications](/examples/RAK4630/communications/)
            - [LoRa](/examples/RAK4630/communications/LoRa/)
                - [LoRaWAN](/examples/RAK4630/communications/LoRa/LoRaWAN/)
                - [LoRa P2P](/examples/RAK4630/communications/LoRa/LoRaP2P/)
            - [BLE](/examples/RAK4630/communications/BLE/)
                - [BLE Proximity Sensing](/examples/RAK4630/communications/BLE/ble_proximity_sensing/)
                - [BLE_OTA_DFU](/examples/RAK4630/communications/BLE/ble_ota_dfu/)
                - [BLE_UART](/examples/RAK4630/communications/BLE/ble_uart/)
                - [BLE_Health](/examples/RAK4630/communications/BLE/BLE_Health/)
                - [RAK12010 BLE](/examples/RAK4630/communications/BLE/RAK12010_Ble/)
                - [RAK12019 BLE](/examples/RAK4630/communications/BLE/RAK12019_UVlight_LTR390_Ble/)
            - [WiFi](/examples/RAK4630/communications/WiFi/)
                - [AT_Command_Test](/examples/RAK4630/communications/WiFi/AT_Command_Test/)
                - [connect_ap](/examples/RAK4630/communications/WiFi/connect_ap/)
        - [RAK4630 Power](/examples/RAK4630/power/)
            - [RAK4630_Battery_Level](/examples/RAK4630/power/RAK4630_Battery_Level_Detect/)
        - [RAK4630 Solutions](/examples/RAK4630/solutions/)
            - [BLE_Gateway](/examples/solutions/BLE_Gateway/)
            - [Hydraulic_Pressure_Monitoring](/examples/RAK4630/solutions/Hydraulic_Pressure_Monitoring/)
            - [Environment_Monitoring](/examples/RAK4630/solutions/Environment_Monitoring/)
            - [Water_Level_Monitoring](/examples/RAK4630/solutions/Water_Level_Monitoring/)
            - [Weather_Monitoring](/examples/RAK4630/solutions/Weather_Monitoring/)
            - [GPS_Tracker](/examples/RAK4630/solutions/GPS_Tracker/)
            - [PAR_Monitoring](/examples/RAK4630/solutions/PAR_Monitoring/)
            - [Soil_Conductivity_Monitoring](/examples/RAK4630/solutions/Soil_Conductivity_Monitoring/)
            - [Soil_pH_Monitoring](/examples/RAK4630/solutions/Soil_pH_Monitoring/)
            - [Wind_Speed_Monitoring](/examples/RAK4630/solutions/Wind_Speed_Monitoring/)
            - [Intelligent_Agriculture](/examples/RAK4630/solutions/Intelligent_Agriculture/)
            - [LoRaWAN_Health](/examples/RAK4630/solutions/LoRaWAN_Health/)
            - [RAK12010 LoRaWan](/examples/RAK4630/solutions/RAK12010_LoRaWan/)
            - [RAK12019 LoRaWan](/examples/RAK4630/solutions/RAK12019_UVlight_LTR390_LoRaWan/)
            - [RAK12035 LoRaWan](/examples/RAK4630/solutions/RAK12035_RAK4631_LPWAN/)

----
    
-    
    - [RAK11200 Examples](/examples/RAK11200/) 
        - [RAK11200 Slot A - D Modules](/examples/RAK11200/sensors/)
            - [RAK18001_Buzzer](/examples/RAK11200/sensors/RAK18001_Buzzer/)
        - [RAK11200 IO Slot Modules](/examples/RAK11200/IO/)
            - [RAK5801_4-20mA](/examples/RAK11200/IO/RAK5801_4-20mA/)
            - [RAK5802_RS485](/examples/RAK11200/IO/RAK5802_RS485/)
            - [RAK5811_0-5V](/examples/RAK11200/IO/RAK5811_0-5V/)
			- [RAK15002_SD_Card](/examples/RAK11200/IO/RAK15002_SD_Card)
			- [RAK17000_Motor_Driver_DRV8833](/examples/RAK11200/IO/RAK17000_Motor_Driver_DRV8833)
			- [RAK17000_Stepper_Driver_DRV8833](/examples/RAK11200/IO/RAK17000_Stepper_Driver_DRV8833)
            - [RAK18000_Stereo](https://github.com/RAKWireless/RAKwireless-Audio-library/tree/main/examples)
			- [RAK19006_WirelessCharger](/examples/RAK11200/IO/RAK19006_WirelessCharger)
        - [RAK11200 Communications](/examples/RAK11200/communications/)
            - [BLE](/examples/RAK11200/communications/BLE/)
                - [BLE Server](/examples/RAK11200/communications/BLE/ble_server/)
                - [BLE UART](/examples/RAK11200/communications/BLE/ble_uart/)
                - [BLE Health](/examples/RAK11200/communications/BLE/BLE_Health/)
                - [RAK12010 BLE](/examples/RAK11200/communications/BLE/RAK12010_Ble/)
                - [RAK12019 BLE](/examples/RAK11200/communications/BLE/RAK12019_UVlight_LTR390_Ble/)
            - [WiFi](/examples/RAK11200/communications/WIFI/)
                - [WiFiAccessPoint](/examples/RAK11200/communications/WIFI/WiFiAccessPoint/)
                - [WiFiClient](/examples/RAK11200/communications/WIFI/WiFiClient/)
            - [LoRa](/examples/RAK11200/communications/LoRa/)
                - [LoRaWAN](/examples/RAK11200/communications/LoRa/LoRaWAN_OTAA_ABP/)
                - [LoRa P2P](/examples/RAK11200/communications/LoRa/RAK13300_LoRaP2P/)
        - [RAK11200 Power](/examples/RAK11200/power/)
            - [RAK11200_Battery_Level_Detect](/examples/RAK11200/power/RAK11200_Battery_Level_Detect/)
        - [RAK11200 Solutions](/examples/RAK11200/solutions/)
            - [Weather Display](/examples/RAK11200/solutions/weather_display/)
            - [mqtt_subscribe_publish](/examples/RAK11200/solutions/mqtt_subscribe_publish/)

----
    
-    
    - [RAK11300 Examples](/examples/RAK11300/) 
        - [RAK11300 Slot A - D Modules](/examples/RAK11300/sensors/)
            - [RAK18001_Buzzer](/examples/RAK11300/sensors/RAK18001_Buzzer/)
        - [RAK11300 IO Slot Modules](/examples/RAK11300/IO/)
            - [RAK5802_RS485](/examples/RAK11300/IO/RAK5802_RS485/)
            - [RAK5801_4-20mA](/examples/RAK11300/IO/RAK5801_4-20mA/)
            - [RAK5811_0-5V](/examples/RAK11300/IO/RAK5811_0-5V/)
			- [RAK15002_SD_Card](/examples/RAK11300/IO/RAK15002_SD_Card)
			- [RAK17000_Motor_Driver_DRV8833](/examples/RAK11300/IO/RAK17000_Motor_Driver_DRV8833)
			- [RAK17000_Stepper_Driver_DRV8833](/examples/RAK11300/IO/RAK17000_Stepper_Driver_DRV8833)
            - [RAK18000_Stereo](https://github.com/RAKWireless/RAKwireless-Audio-library/tree/main/examples)
			- [RAK19006_WirelessCharger](/examples/RAK11300/IO/RAK19006_WirelessCharger)
        - [RAK11300 Power](/examples/RAK11300/power/)
            - [RAK11300_Battery_Level_Detect](/examples/RAK11300/power/RAK11300_Battery_Level_Detect/)
        - [RAK11300 Communications](/examples/RAK11300/communications/)
            - [LoRa](/examples/RAK11300/communications/LoRa/)
                - [LoRaWAN](/examples/RAK11300/communications/LoRa/LoRaWAN/)
                - [LoRa P2P](/examples/RAK11300/communications/LoRa/LoRaP2P/)
                - [AT Command](/examples/RAK11300/communications/LoRa/AT-Command/)
            - [WiFi](/examples/RAK11300/communications/WIFI/)
                - [AT_Command_Test](/examples/RAK11300/communications/WIFI/AT_Command_Test/)
                - [connect_ap](/examples/RAK11300/communications/WIFI/connect_ap/)
        - [RAK11300 Solutions](/examples/RAK11300/solutions/)
            - [Hydraulic_Pressure_Monitoring](/examples/RAK11300/solutions/Hydraulic_Pressure_Monitoring/)
            - [Environment_Monitoring](/examples/RAK11300/solutions/Environment_Monitoring/)
            - [Water_Level_Monitoring](/examples/RAK11300/solutions/Water_Level_Monitoring/)
            - [Weather_Monitoring](/examples/RAK11300/solutions/Weather_Monitoring/)
            - [GPS_Tracker](/examples/RAK11300/solutions/GPS_Tracker/)
            - [PAR_Monitoring](/examples/RAK11300/solutions/PAR_Monitoring/)
            - [Soil_Conductivity_Monitoring](/examples/RAK11300/solutions/Soil_Conductivity_Monitoring/)
            - [Soil_pH_Monitoring](/examples/RAK11300/solutions/Soil_pH_Monitoring/)
            - [Wind_Speed_Monitoring](/examples/RAK11300/solutions/Wind_Speed_Monitoring/)
            - [Intelligent_Agriculture](/examples/RAK11300/solutions/Intelligent_Agriculture/)
            - [LoRaWAN_Health](/examples/RAK11300/solutions/LoRaWAN_Health/)
            - [RAK12010 LoRaWan](/examples/RAK11300/solutions/RAK12010_LoRaWan/)
            - [RAK12019 LoRaWan](/examples/RAK11300/solutions/RAK12019_UVlight_LTR390_LoRaWan/)
    
----

- Bootloader/Firmware
    - [RAK4630](bootloader/RAK4630)
        - [Flashable Bootloader](bootloader/RAK4630/Latest)
        - [Bootloader Source Code](bootloader/RAK4630/Latest/WisCore_RAK4631_Bootloader)
    - [RAK2305 AT firmware](bootloader/RAK2305)
----

- PlatformIO installation
   - [Patch for PIO installation](PlatformIO)
   - [RAK4630 PlatformIO files](PlatformIO/RAK4630/)
   - [RAK11200 PlatformIO files](PlatformIO/RAK11200/)
   - [RAK11300 PlatformIO files](PlatformIO/RAK11300/)
----


## Overview
WisBlock is a modular Plug&Play system. Compared with other systems (Arduino, Raspberry Pi, M5Stack), it has several advantages:

1. Using a compact connector makes its size very small. A WisBlock Base board, which is the base carrier, is only 30mm by 60mm in size. One WisBlock Base can hold one micro-controller module (WisBlock Core), one IO module (WisBlock IO) and up to 4 sensor modules (WisBlock Sensor).

2. Using an industrial rated connector enables you to use the same modules from rapid prototyping to testing to final product.  

3. WisBlock is not only modular on hardware base. With ready to use software blocks, it is simple to create an application to match your implementation requirements.

----
## Modularity
### Base board
WisBlock Base is the base board that makes flexibility and modularity possible.

### Plug&Play hardware modules 
WisBlock Core → processing block with LoRa/LoRaWan, WiFi or BLE connectivity.    
WisBlock Sensor → wide range of sensor blocks.    
WisBlock IO → blocks for user interfaces, custom sensor connections and actuators.

### Plug&Play software modules
Using Arduino framework, WisBlock provides a free and widely available programming environment:
- Open Source libraries for communication protocols
- Open Source libraries for data processing
- Open Source libraries for sensors input
- Open Source libraries for actuators
- Open Source libraries for user interfaces

----
### WisBlock Base
WisBlock is build up on a WisBlock Base board. It is a platform carrier that allows easy plug-in of one WisBlock Core processing board, one WisBlock IO interface board and up to four WisBlock Sensor boards.

WisBlock Base is providing the power supply, battery and solar panel connections and charging block. In addition it has connectors for programming and debugging. 

During development phase, WisBlock Base allows you to quickly switch between microcontrollers, IO functionality and sensors by simply changing the modules.

Repairs and upgrades are easy with WisBlock Base on your final product, because modules can be changed with just Plug'n'Play.

For controlling and limiting power consumption of your IoT solution, WisBlock Base enables to control the power supply for the WisBlock Sensor and WisBlock IO modules, limiting the power consumption by switching off these modules if they are not needed.

----
### WisBlock Core
WisBlock Core is the processing unit of your IoT solution.

Select the processing power based on the requirements of your application from a range of processing boards which starts with low single core to high dual core processing power units are available. 

All of them communication capability, some of them offer in addition LoRa/LoRaWAN, WiFi, Bluetooth or Bluetooth Low Energy. All are designed for battery optimized low power consumption.

----
### WisBlock Sensor
A range of sensors for sensing environmental conditions (temperature, humidity, …), location and movement conditions (GNSS/GPS, accelerometer, …) that you just plug into WisBlock Base makes it easy to complete your application with the required sensors.

----
### WisBlock IO
WisBlock IO extensions provide your application with interfaces that are not covered by WisBlock Core or WisBlock Sensor blocks.     
This includes 
- user interfaces like 
  - keyboards
  - buttons
  - displays
- communication interfaces like    
  - 0~5V
  - 4-20mA
  - I2C
  - RS232
  - RS485
  - many more
- alternative power supplies like
  - 24V
  - POE
