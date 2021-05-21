# LoRa®  


## 1. Introduction

This guide explains the LoRaWAN®  modules features and functions. These products were designed for RAKwireless WisBlock, including core, sensor, baseboard and extended IO, so now you can create your own Low Power Wide Area Network (LPWAN).

LoRaWAN®  is a new, private and spread-spectrum modulation technique which allows sending data at extremely low data-rates to extremely long ranges. The low data-rate (down to few bytes per second) and LoRaWAN®  modulation lead to very low receiver sensitivity (down to -136 dBm), which combined to an output power of +14 dBm means extremely large link budgets: up to 150 dB, what means more than 22 km (13.6 miles) in LOS links and up to 2 km (1.2 miles) in NLOS links in urban environment (going through buildings).

There are two basic usages, first is LoRaWAN® . WisBlock works as node, and report to LoRaWAN®  gateway. Then gateway transmit to server on the cloud, like TTN/Lora Server, finally to users. Besides WisBlock, RAKwireless provide a whole solution from node to server.

![wifi-lora-net](../../../../assets/Examples/wifi-lora-net.png)

Second is based on P2P mode. Nodes may connect directly among them and send messages directly at no cost (as they are not using the LoRaWAN®  Network but just direct radio communication). For more info go to the section P2P Mode.



## 2. LoRa Example

Here you find several examples how to use the LoRa® communication with WisBlock. The examples are separated into two categories:     

- Simple [LoRa® point to point communication](./LoRaP2P/)      
- Using [LoRaWAN® ](./LoRaWAN/)


LoRa® is a registered trademark or service mark of Semtech Corporation or its affiliates. LoRaWAN® is a licensed mark.