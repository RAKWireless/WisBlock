# RAKwireless WisBlock
| <img src="assets/repo/RAK.png" alt="Modules" width="150"> | <img src="assets/repo/rakstar.jpg" alt="Modules" width="100"> | <img src="assets/repo/rak11200.png" alt="Modules" width="250"> | <img src="assets/repo/RAK4631.png" alt="Modules" width="250"> | [![Build Status](https://github.com/RAKWireless/WisBlock/workflows/RAK%20BSP%20Arduino%20Build%20CI/badge.svg)](https://github.com/RAKWireless/WisBlock/actions) |    
| :-: | :-: | :-: | :-: | :-: |  

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
- Documentation  
    - [Quick Start Guide RAK4630](quickstart/RAK4630)
    - [Quick Start Guide RAK11200](quickstart/RAK11200)
    - [Quick Start Guide WisBlock Base](quickstart/Base)
    - [Quick Start Guide WisBlock IO Modules](quickstart/IOModules)
    - [Quick Start Guide WisBlock Sensor Modules](quickstart/SensorModules)
----

- [Examples](/examples/)    
    - [Common to all WisBlock Cores](/examples/common/)
        - [Sensors Modules](/examples/common/sensors/)
            - [RAK1901_Temperature&Humidity_SHTC3](/examples/common/sensors/RAK1901_Temperature_Humidity_SHTC3/)
            - [RAK1902_Pressure_LPS22HB](/examples/common/sensors/RAK1902_Pressure_LPS22HB/)
            - [RAK1903_Optical_OPT3001](/examples/common/sensors/RAK1903_Optical_OPT3001/)
            - [RAK1904_Accelerate_LIS3DH](/examples/common/sensors/RAK1904_Accelerate_LIS3DH/)
            - [RAK1906_Environment_BME680](/examples/common/sensors/RAK1906_Environment_BEM680/)
            - [RAK1910_GPS_UBLOX7](/examples/common/sensors/RAK1910_GPS_UBLOX7/)
            - [RAK1920_MikroBUS_Temperature_TMP102](/examples/common/sensors/RAK1920_MikroBUS_Temperature_TMP102/)
            - [RAK1920_QWIIC_AirQuality_SGP30](/examples/common/sensors/RAK1920_QWIIC_AirQuality_SGP30/)
            - [RAK1920_Grove_PIR_AS312](/examples/common/sensors/RAK1920_Grove_PIR_AS312/)
            - [RAK12003_FIR_MLX90632](/examples/common/sensors/RAK12003_FIR_MLX90632/)
            - [RAK12500_GPS_ZOE-M8Q](/examples/common/sensors/RAK12500_GPS_ZOE-M8Q/)
            - [RAK15000_EEPROM_AT24C02](/examples/common/sensors/RAK15000_EEPROM_AT24C02/)     
            - [RAK15001_Flash_GD25Q16C](/examples/common/sensors/RAK15001_Flash_GD25Q16C/)     
            - [RAK19002_Boost_TPS61046](/examples/common/sensors/RAK19002_Boost_TPS61046/)     
        - [IO Modules](/examples/IO/)
            - [RAK1921_OLED_SSD1306](/examples/common/IO/RAK1921_OLED_SSD1306/)
            - [RAK1921_Jumping_Ball_SSD1306](/examples/common/IO/RAK1921_Jumping_Ball_SSD1306/)
            - [RAK1921_Moving_Logo_SSD1306](/examples/common/IO/RAK1921_Moving_Logo_SSD1306/)
            - [RAK14000-Epaper-Monochrome](/examples/common/IO/RAK14000-Epaper-Monochrome/)
            - [RAK14000-Epaper-TriColor](/examples/common/IO/RAK14000-Epaper-Monochrome/)
        - [Communications](/examples/common/communications/)
            - [Cellular](/examples/common/communications/Cellular/)
                - [BG77_Unvarnished_Transmission](/examples/common/communications/Cellular/BG77_Unvarnished_Transmission)
                - [Cellular_Ping](/examples/common/communications/Cellular/Cellular_Ping)
                - [Hologram_Tcp](/examples/common/communications/Cellular/Hologram_Tcp)
                - [Hologram_Ping_LTE](/examples/common/communications/Cellular/Hologram_Ping_LTE)
    - [RAK4630 Examples](/examples/RAK4630/) 
        - [RAK4630 Sensors](/examples/RAK4630/sensors/)
            - [RAK18001_Buzzer](/examples/RAK4630/sensors/RAK18001_Buzzer/)
        - [RAK4630 IO](/examples/RAK4630/IO/) 
            - [RAK5802_RS485](/examples/RAK4630/IO/RAK5802_RS485/)
            - [RAK5801_4-20mA](/examples/RAK4630/IO/RAK5801_4-20mA/)
            - [RAK5811_0-5V](/examples/RAK4630/IO/RAK5811_0-5V/)    
            - [RAK18000_Stereo](/examples/RAK4630/IO/RAK18000_Stereo/)
        - [RAK4630 Communications](/examples/RAK4630/communications/)
            - [LoRa](/examples/RAK4630/communications/LoRa/)
                - [LoRaWAN](/examples/RAK4630/communications/LoRa/LoRaWAN/)
                - [LoRa P2P](/examples/RAK4630/communications/LoRa/LoRaP2P/)
            - [BLE](/examples/RAK4630/communications/BLE/)
                - [BLE Proximity Sensing](/examples/RAK4630/communications/BLE/ble_proximity_sensing/)
                - [BLE_OTA_DFU](/examples/RAK4630/communications/BLE/ble_ota_dfu/)
                - [BLE_UART](/examples/RAK4630/communications/BLE/ble_uart/)
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
            
    - [RAK11200 Examples](/examples/RAK11200/) 
        - [RAK11200 Sensors](/examples/RAK11200/sensors/)
            - [RAK18001_Buzzer](/examples/RAK11200/sensors/RAK18001_Buzzer/)
        - [RAK11200 IO](/examples/RAK11200/IO/)
            - [RAK5802_RS485](/examples/RAK11200/IO/RAK5802_RS485/)
            - [RAK5801_4-20mA](/examples/RAK11200/IO/RAK5801_4-20mA/)
            - [RAK5811_0-5V](/examples/RAK11200/IO/RAK5811_0-5V/)
            - [RAK18000_Stereo](/examples/RAK11200/IO/RAK18000_Stereo/)
        - [RAK11200 Communications](/examples/RAK11200/communications/)
            - [BLE](/examples/RAK11200/communications/BLE/)
                - [ble_server](/examples/RAK11200/communications/BLE/ble_server/)
                - [ble_uart](/examples/RAK11200/communications/BLE/ble_uart/)
            - [WiFi](/examples/RAK11200/communications/WIFI/)
                - [WiFiAccessPoint](/examples/RAK11200/communications/WIFI/WiFiAccessPoint/)
                - [WiFiClient](/examples/RAK11200/communications/WIFI/WiFiClient/)
        - [RAK11200 Power](/examples/RAK11200/power/)
            - [RAK4630_Battery_Level](/examples/RAK11200/power/RAK4630_Battery_Level_Detect/)
        - [RAK11200 Solutions](/examples/RAK11200/solutions/)
            - [Weather Display](/examples/RAK11200/solutions/weather_display/)
            - [mqtt_subscribe_publish](/examples/RAK11200/solutions/mqtt_subscribe_publish/)
    
----

- Bootloader/Firmware
    - [RAK4630](bootloader/RAK4630)
        - [Flashable Bootloader](bootloader/)
        - [Bootloader Source Code](bootloader/Adafruit_nRF52_Bootloader/)
    - [RAK2305](bootloader/ RAK2305_ESP32_WROVER)
----

- PlatformIO installation
   - [RAK4630 PlatformIO files](PlatformIO/RAK4630/)
   - [RAK11200 PlatformIO files](PlatformIO/RAK11200/)
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
