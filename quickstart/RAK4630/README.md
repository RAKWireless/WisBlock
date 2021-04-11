# WisBlock Quick Start Guide

<center><img src="../../assets/repo/WisBlock-Assembly.png" alt="WisBlock-Assembly" width="75%" /></center>

## Content
- [Introduction](#introduction)    
- [Safety information](#safety-information)    
- [Hardware Setup](#hardware-setup)
- [Arduino IDE BSP Installation](#arduino-ide-bsp-installation)
- [Load Examples](#load-examples)

## Introduction

WisBlock is an amazing product built by RAK company for IoT industry. It can build circuits like building blocks quickly to realize your idea, and through high-speed connectors and fasteners interconnection, it can directly compose the reliable industrial products.

WisBlock consists of WisBlock Base, WisBlock Core, WisBlock Sensor, and WisBlock IO.

RAK4631 is the WisBlock Core board which consists of nRF52840 and SX1262. It supports LoRa® and BLE functions, and supply a rich resource MCU so that you can program it if you want.

WisBlock is not only a functional test capable product in product development verification stage, but also industrial products oriented to mass production. It uses a high-speed connector to ensure the integrity of the signal. At the same time, it is equipped with fastening screw, which can be used in vibration environment. And WisBlock can be used reliably in various civil and industrial scenarios through rigorous reliability tests.

WisBlock uses a compact stacked hardware design, which integrates various computing, connecting and sensor circuits in the size of 60*30mm. The compact size makes it easy for users to build in various customized housings to achieve complete products. RAK also have a series of housings for WisBlock modules, which can meet the requirements of various protection levels.

More details about WisBlock hardware, please have a look at the document:

https://docs.rakwireless.com/Product-Categories/WisBlock/

## Safety information

Please read the following items carefully so that WisBlock can be used safely.

### Hardware

1) Please use WisBlock according to its hardware specification, including the power supply, the temperature of using, the battery, and so on.

2) Don't submerge WisBlock in liquids, and don't place WisBlock where water can reach.

3) Don't power WisBlock using other power sources which RAK hasn't suggested.



### Software

1)  There is already a bootloader in every WisBlock core board MCU when you receive the device, so that you needn't to flash the bootloader again. Normally, you only need to use it directly or upload new code into it through Arduino IDE. If you accidentally erase the bootloader, please contact with us on RAK forum: forum.rakwireless.com.

2) Please don't unplug any hardware connector when you are uploading code into it, otherwise WisBlock may become unresponsive.



## Hardware Setup


### WisBlock Core board --- RAK4631

RAK4631 is the WisBlock Core board, because the MCU stay on this board.

<center><img src="../../assets/repo/RAK4631.png" alt="RAK4631" width="30%" /></center>
<center><img src="../../assets/repo/rak4631-connectors.png" alt="rak4631-connectors" width="60%" /></center>

RAK4631 consists of a nRF52840 MCU and a SX1262 LoRa® chip mainly. The final Arduino  code will be uploaded into nRF52840 actually. This core board supports BLE and LoRa® features. You can connect it with RAK5005-O base board with slot, and program it through the USB interface on RAK5005-O through Arduino IDE.

https://docs.rakwireless.com/Product-Categories/WisBlock/RAK4631/Datasheet/#overview

<center><img src="../../assets/repo/RAK5005-connect-RAK4631.PNG" alt="RAK5005-connect-RAK4631" width="75%" /></center>

**RAK5005-O GPIO mapping to RAK4631 GPIO ports**
 * RAK5005-O <->  nRF52840
 * IO1       <->  P0.17 (Arduino GPIO number 17)
 * IO2       <->  P1.02 (Arduino GPIO number 34)
 * IO3       <->  P0.21 (Arduino GPIO number 21)
 * IO4       <->  P0.04 (Arduino GPIO number 4)
 * IO5       <->  P0.09 (Arduino GPIO number 9)
 * IO6       <->  P0.10 (Arduino GPIO number 10)
 * SW1       <->  P0.01 (Arduino GPIO number 1)
 * A0        <->  P0.04/AIN2 (Arduino Analog A2
 * A1        <->  P0.31/AIN7 (Arduino Analog A7
 * SPI_CS    <->  P0.26 (Arduino GPIO number 26) 

## Arduino IDE BSP Installation

You can use Arduino IDE for WisBlock coding and programming.

The RAK4631 is included in the RAK Arduino BSP.       
You can find the installation instructions in our [RAK-nRF52-Arduino](https://github.com/RAKWireless/RAK-nRF52-Arduino#readme) repository

### Load Examples

RAK has supplied many examples source code on Github for WisBlock:

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
