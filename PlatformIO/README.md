<img src="assets/rakstar.jpg" alt="RAKstar" height="150"><img src="assets/RAK-Whirls.png" alt="RAKwireless" height="150">
# RAK WisBlock modules in PlatformIO

## ⚠️ Update
While the Python script to add WisDuo and WisBlock boards to the PlatformIO BSP's is still working, the problem with the script is that it has to be run every time PlatformIO updates the platforms.     
To get around this, I checked the possibilities of PlatformIO to add custom boards to existing platforms, and I found a different solution, inspired by the work of [Meshtastic](https://github.com/meshtastic/firmware). They are using PlatformIO's features to add (or better add customized) additional boards to the existing platforms.    
RAK_PATCH_V2 is the result of my findings.    

# RAK_PATCH_V2
Instead of adding board descriptions and files to the existing platforms, the boards are added directly in the project folders.     
This keeps the added boards active, even if the main platform is updated.

# ⚠️ IMPORTANT:
_**For the RAK11300 and RAK11310 modules with the RP2040 MCU, this method works only with the develop branch of [platform-raspberrypi](https://github.com/maxgerhardt/platform-raspberrypi/tree/develop) that is using the RP2040 BSP [arduino-pico](https://github.com/earlephilhower/arduino-pico) provided by [Earl F. Philhower, III](https://github.com/earlephilhower)**_

## How does it work?
PlatformIO allows to add custom board definition files (*.json) and custom variant folders to the main platform by hosting them in the project folder.    
To use this feature, 2 steps are required in the project folder:    

1) Copy the folder _**rakwireless**_ into the project folder. This folder contains the WisBlock and WisDuo board definitions and board variant files.
2) Add access to these board definitions and variant files in the platformio.ini file of the project.

The folder _**rakwireless**_ and an example platformio.ini file are provided in the archive [_**RAK_PATCH_V2.zip**_](./RAK_PATCH_V2.zip). Download this archive for the next steps.    

## Step 1
Copy the complete folder _**rakwireless**_ from the _**RAK_PATCH_V2.zip**_ archive into your project folder. The project folder structure should look like:    

- EXAMPLE_PROJECT
   - .pio
   - .vscode
   - include
   - lib
   - _**rakwireless**_
   - src
   - test
   - .platformio.ini

## Step 2
To add these "custom" boards to the project, a few entries are required in the platformio.ini file:

- _**boards_dir**_ to define the path to the additional boards (only needed for RAK4630/RAK4631, RAK11200 and RAK3112/RAK3312)    
- _**build_src_filter**_ to add the variant cpp files to tbe build path (Only needed for RAK4630/RAK4631)    
- _**build_flags = -I rakwireless/variants/rakxxxxxx**_ to include the variant include files (only needed for RAK4630/RAK4631, RAK11200 and RAK3112/RAK3312)    

An example that adds all 4 available Arduino based WisBlock Core modules and WisDuo stamp modules to a project can be found in the [platformio.ini](./RAK_PATCH_V2/rakwireless/platformio.ini) file. 

### platformio.ini for RAK4630/RAK4631
To use the WisBlock Core RAK4631 or WisDuo RAK4630 stamp module the platformio.ini file should content the following entries:

```ini
[platformio]
boards_dir = rakwireless/boards
; other required definitions

[env:rak4630]
platform = nordicnrf52
board = rak4630 
framework = arduino
build_src_filter = ${env.build_src_filter}+<../rakwireless/variants/rak4630>
build_flags = 
	-I rakwireless/variants/rak4630
	; other build flags
lib_deps = 
	; project library dependencies
; other required environment definitions
```

### platformio.ini for RAK11200
To use the WisBlock Core RAK11200 the platformio.ini file should content the following entries:

```ini
[platformio]
boards_dir = rakwireless/boards
; other required definitions

[env:rak11200]
platform = espressif32
board = rak11200 
framework = arduino
build_flags = 
	-I rakwireless/variants/rak11200
	; other build flags
lib_deps = 
	; project library dependencies
; other required environment definitions
```

### platformio.ini for RAK3112/RAK3312
To use the WisBlock Core RAK3312 or WisDuo RAK3112 stamp module the platformio.ini file should content the following entries:

```ini
[platformio]
boards_dir = rakwireless/boards
; other required definitions

[env:rak11200]
platform = espressif32
board = RAK3112 
framework = arduino
build_flags = 
	-I rakwireless/variants/RAK3112
	; other build flags
lib_deps = 
	; project library dependencies
; other required environment definitions
```

### platformio.ini for RAK11300/RAK11310
⚠️ The develop branch of the [platform-raspberrypi](https://github.com/maxgerhardt/platform-raspberrypi/tree/develop) already has a variant of for the RAK11300/RAK11310. But the variant definition is missing the WisBlock GPIO definitions that are used in the RAKwireless examples.    

The platfromio.ini example is using the _**rakwireless_rak11300**_ board, but adds the WisBlock GPIO definitions in the build_flags.    

To add the missing WisBlock GPIO definitions with the WisBlock Core RAK11310 or WisDuo RAK11300 stamp module the platformio.ini file should content the following entries:

```ini
[platformio]
boards_dir = rakwireless/boards
; other required definitions

[env:rak11300]
platform = https://github.com/maxgerhardt/platform-raspberrypi
board_build.core = earlephilhower
board = rakwireless_rak11300
framework = arduino
build_flags = 
	-D ARDUINO_RAKWIRELESS_RAK11300=1
	; WisBlock definitions
	-I rakwireless/variants/rak11300
	-D WB_IO1=6
	-D WB_IO2=22
	-D WB_IO3=7
	-D WB_IO4=28
	-D WB_IO5=9
	-D WB_IO6=8
	-D WB_A0=26
	-D WB_A1=27
	-D PIN_LED1=23
	-D PIN_LED2=24
	-D LED_BUILTIN=23
	-D LED_CONN=24
	-D LED_GREEN=23
	-D LED_BLUE=24
	; other build flags
lib_deps = 
	; project library dependencies
; other required environment definitions
```

## Example projects for RAK_PATCH_V2

Two examples are available to show the usage of the patch.    
The first example [WisBlock-LoRaWAN-OTAA](./RAK_PATCH_V2/examples/WisBlock-LoRaWAN-OTAA/) is a simple example using the SX126x-Arduino library to establish an OTAA based connection to a LoRaWAN server.    
The second example [WisBlock-API-V2-Example](./RAK_PATCH_V2/examples/WisBlock-API-V2-Example/) is based on the [WisBlock-API-V2](https://github.com/beegee-tokyo/WisBlock-API-V2) API, that provides low power coding and an AT command interface to setup the devices.    

Both examples work on the RAK4630/RAK4631, RAK11300/RAK11310, RAK3112/RAK3312 and RAK11200 + RAK13300 modules.    

# RAK_PATCH (old version)
A small script that installs or updates RAKwireless WisBlock modules in PlatformIO BSP's    
This script does not install the required BSP's (platform and package), it only patches already installed BSP's.

_**REMARK**_
Tested only on Windows 10 and Ubuntu!

- [First install](#first-install)
- [Update](#update)
- [Download](#download)

## First install
Before you can use the WisBlock Core modules with PlatformIO you need to install the original platforms:
- RAK4631 => install nRF52 platform    
<img src="assets/Platform-nRF52.png" alt="nRF52 platform" height="150">
- RAK11200 => install ESP32 platform
<img src="assets/Platform-ESP32.png" alt="nRF52 platform" height="150">
- RAK11300 => install Raspberry Pi RP2040 platform
<img src="assets/Platform-RP2040.png" alt="nRF52 platform" height="150">

After the required platforms are installed, follow the steps as shown in [Update](#update)

## Update
After updating or installing the nRF52, ESP32 or Raspberry Pi RP2040 package do the following steps:

### 1. Open a project that uses the updated platform

### 2. Compile the project so that PlatformIO is installing the packages belonging to the platform
   - Ignore the _**Unknown board ID**_ or _**variant.h: No such file or directory**_ errors until the following patch has been applied

### 3. Patch the platform and package   

Unzip the contents of **RAK_PATCH.zip** into folder RAK_PATCH in your PlatformIO installation folder.    
   
| Paths on different OS: |    |    
| --- | --- |    
| Windows | `%USER%\.platformio\` | 
| Linux | `~/.platformio/` | 
| MacOS | `/Users/{Your_User_id}/.platformio/` | 
   
**Example (Windows 10):**    
<img src="assets/folder-view.png" alt="PlatformIO" height="150">

**Install the patch**    

Open a terminal in the .platformio folder and execute **`python ./rak_patch.py`**

Example (Windows 10 terminal):    

<img src="assets/patch_windows.png" alt="Windows 10" height="150">

Example (Linux terminal):    

<img src="assets/patch_linux.png" alt="Linux" height="150">

# Download

Download the complete package from [RAK_PATCH.zip](https://raw.githubusercontent.com/RAKWireless/WisBlock/master/PlatformIO/RAK_PATCH.zip)
