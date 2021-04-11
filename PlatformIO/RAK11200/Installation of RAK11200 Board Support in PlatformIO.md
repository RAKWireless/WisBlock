## Installation of RAK11200 Board Support in PlatformIO(Just for test now)

### Install PlatformIO

First of all, install VS code which is a great and open source tool, and you can download it here:

https://code.visualstudio.com/

After installing VS code, you can search PlatformIO and install it in the Extensions item.

### Install  Espressif 32 Arduino framework

After installing PlatformIO, you can see the PlatformIO icon and open it as follow

Open "Platforms" in PlatformIO and search "Espressif" as follow

You can see there are several items, just click "Espressif 32" item and "Install" it as follow

### Add WisBlock Core RAK11200 to the platform

Copy the file **`wiscore_rak11200.json`** to the boards folder of the espressif32 platform folder.

The path maybe like:

- Windows: %USER%.platformio\platforms\espressif32\boards
### Add WisBlock Core RAK11200 to the framework

Refer to the modification method of Arduino IDE 

The path of framework maybe like:

- Windows: %USER%.platformio\packages\framework-arduinoespressif32

  

