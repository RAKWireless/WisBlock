## Installation of RAK11300 Board Support in PlatformIO(Just for test now)

### Install PlatformIO

First of all, install VS code which is a great and open source tool, and you can download it here:

https://code.visualstudio.com/

After installing VS code, you can search PlatformIO and install it in the Extensions item.

### Install  Raspberry Pi RP2040 Arduino framework

After installing PlatformIO, you can see the PlatformIO icon and open it as follow

Open "Platforms" in PlatformIO and search "Raspberry Pi RP2040" as follow

You can see there are several items, just click "Raspberry Pi RP2040" item and "Install" it as follow

### Add WisBlock Core RAK11300 to the platform

Copy the file **`rak11300.json`** to the boards folder of the Raspberry Pi RP2040 platform folder.

The path should be similar to:
```
- Windows: %USER%.platformio\platforms\raspberrypi\boards
- Linux:    ~/.platformio/platforms/raspberrypi/boards
- Mac OS:  /Users/{Your_User_id}/.platformio/platforms/raspberrypi/boards
```

### Add WisBlock Core RAK11300 to the framework

Refer to the modification method of Arduino IDE 

The path should be similar to:
```
- Windows: %USER%.platformio\packages\framework-arduino-mbed
- Linux:    ~/.platformio/packages/framework-arduino-mbed
- Mac OS:  /Users/{Your_User_id}/.platformio/packages/framework-arduino-mbed
```
  

