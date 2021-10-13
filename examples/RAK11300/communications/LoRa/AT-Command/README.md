# RAK11300 AT Command Firmware

| <center><img src="./assets/rakstar.jpg" alt="RAKstar" width="250"></center>  | <center><img src="./assets/RAK-Whirls.png" alt="RAKWireless" width="250"></center> | <center><img src="./assets/WisBlock.png" alt="WisBlock" width="250"></center> | <center><img src="./assets/Yin_yang-48x48.png" alt="BeeGee" width="250"></center>  | <center><img src="./assets/RAK11310.png" alt="RAK11310" width="250"></center>  |
| -- | -- | -- | -- | -- |

----

This is the source code of the default firmware you find on your RAK11300.    
This project is a quick start for the new [RAKwireless WisDuo RAK11300 LPWAN stamp module](https://docs.rakwireless.com/Product-Categories/WisDuo/). It gives the opportunity to test the LPWAN functionality without writing and flashing code.    
It is a very simple firmware that provides an AT Command Interface over USB and RX1/TX1 UART of the module. It is not optimized for low power consumption.

----

The LPWAN credentials are stored in the flash memory of the RAK11300 LPWAN stamp module. The firmware is written using the Arduino framework. It uses the [RAK-RP-Arduino BSP](https://github.com/RAKWireless/RAK-RP-Arduino) which supports the RAK11300/RAK11310. 

_**REMARK**_  
_This firmware uses some functions to access the flash memory that are not in the current Arduino BSP (V2.5.2). You need to copy the file `variant.cpp` from `Patch for BSP` into the `variants\WisBlock_RAK11300_Board` folder of the installed BSP.    
This is not required on newer versions of the BSP_

----

All AT commands are listed and explained in the [AT-Command Manual](./AT-Commands.md). 

## Short example for OTAA connection test:    
Set the DevEUI
```
AT+DEVEUI=5555555553525150
```
Set the AppEUI
```
AT+APPEUI=5051525355555555
```
Set the AppKey
```
AT+APPKEY=55555555535251505051525355555555
```
Set the Region (US915)
```
AT+BAND=8
```
Check the settings
```
AT+STATUS=?
```
Start network join (requires a gateway connected to a LPWAN server and the device registered on the LPWAN server with above credentials)
```
AT+JOIN=1,0,1,1
```

Log output:    
```
============================
RAK11300 AT Command Firmware
SW Version 1.0.0
LoRa(R) is a registered trademark or service mark of Semtech Corporation or its affiliates.LoRaWAN(R) is a licensed mark.
============================
LoRaWAN status:
   Auto join disabled
   OTAA enabled
   Dev EUI 000D75E6564DC1F3
   App EUI 1200353833333250
   App Key 2B84E0B09B68E5CB42176FE753DCEE79
   NWS Key 50323333383500121200353833333250
   Apps Key 50323333383500121200353833333250
   Dev Addr 83986D12
   Repeat time 120000
   ADR disabled
   Public Network
   Dutycycle disabled
   Join trials 1
   TX Power 0
   DR 3
   Class 0
   Subband 1
   Fport 2
   Confirmed Message
   Region 10
   Network not joined
============================
AT+JOIN=1:0:30:10
AT+JOIN=1:0:30:10

OK

AT+JOIN=SUCCESS
```