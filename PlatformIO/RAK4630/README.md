# Installation of Board Support Package in PlatformIO

## Install PlatformIO

First of all, install VS code which is a great and open source tool, and you can download it here:

https://code.visualstudio.com/

After installing VS code, you can search PlatformIO and install it in the Extensions item:

![pio_install_pio_1](../../assets/PlatformIO/pio_install_pio_1.png)

After installing PlatformIO, you can see the PlatformIO icon and open it as follow:

![pio_open_pio_home](../../assets/PlatformIO/pio_open_pio_home.png)

Open "Platforms" in PlatformIO and search "Nordic" as follow:

![pio_install_platform_1](../../assets/PlatformIO/pio_install_platform_1.png)

You can see there are several items, just click "Nordic nRF52" item and "Install" it as follow:

![pio_install_platform_2](../../assets/PlatformIO/pio_install_platform_2.png)

![pio_install_platform_3](../../assets/PlatformIO/pio_install_platform_3.png)

Download the folder WisCore_RAK4631_board and the file wiscore_rak_4631.json from this repository

Copy the file **`wiscore_rak4631.json`** to the boards folder of the nordicnrf52 platform folder.

The path should be similar to:

```
Windows: %USER%\.platformio\platforms\nordicnrf52\boards
Linux:    ~/.platformio/platforms/nordicnrf52/boards
Mac OS:  /Users/{Your_User_id}/.platformio/platforms/nordicnrf52/boards
```

![pio_folder_location](../../assets/PlatformIO/pio_folder_location_rak4631.png)

Then, create a new project in PlatformIO:

![pio_install_platform_4](../../assets/PlatformIO/pio_install_platform_4.png)

Choose "WisCore RAK4631 Board (RAKwireless)" for "Board" item, and choose "Arduino" for "Framework" item as follow:

![pio_install_platform_5](../../assets/PlatformIO/pio_install_platform_5.png)

After creating successfully, you can see the project:

![pio_install_platform_6](../../assets/PlatformIO/pio_install_platform_6.png)

Now, Copy the complete folder "WisCore_RAK4631_Board" in the "PlatformIO" folder which you just downloaded to the framework-arduinoadafruitnrf52 package variants folder. The path should be similar to:
```
 - Windows: %USER%\\.platformio\packages\framework-arduinoadafruitnrf52\variants
 - Linux:    ~/.platformio/packages/framework-arduinoadafruitnrf52/variants
 - Mac OS:  /Users/{Your_User_id}/.platformio/packages/framework-arduinoadafruitnrf52/variants
```

![pio_folder_location](../../assets/PlatformIO/pio_folder_location_rak4631.png)

Finally, restart the PlatformIO.

OK, you've install and configure PlatformIO for WisBlock successfully.



Now, let's try to compile an example of WisBlock using PlatformIO. We use the LoRaWAN® OTAA example in this document:

https://github.com/RAKWireless/WisBlock/tree/master/examples/communications/LoRa/LoRaWAN/LoRaWAN_OTAA/

Just copy the source code of the .ino file into the main.cpp of the PlatformIO project we just created:

![pio_compile_1](../../assets/PlatformIO/pio_compile_1.png)

Then we need to install the LoRaWAN® library "SX126x-Arduino" in PlatformIO firstly because this example is built based on this library. Just search "SX126x" in "Libraries" item of PlatformIO, and you can see "SX126x-Arduino" as follow:

![pio_compile_2](../../assets/PlatformIO/pio_compile_2.png)

Just click it and "Install" this library as follow:

![pio_compile_3](../../assets/PlatformIO/pio_compile_3.png)

To define your LoRaWan regional settings, open the file `platformio.ino` and set the region as a build-flag. In this example we set it to US915:
```ini
build_flags = 
    -DREGION_US915
```
And here we set the region to EU868:
```ini
build_flags = 
    -DREGION_EU868
```

![pio_compile_6](../../assets/PlatformIO/pio_compile_6.png) 

Then compile it by click the compiling icon at the bottom tool bar as follow:

![pio_compile_4](../../assets/PlatformIO/pio_compile_4.png) 

![pio_compile_5](../../assets/PlatformIO/pio_compile_5.png)

Great! We've compiled this example successfully!

Same as in Arduino IDE, there is an upload icon (green circle) on the right of the compiling icon (red circle) which can be used to upload the compiled firmware into your device.

![pio-flash](../../assets/PlatformIO/pio-flash.jpg)
