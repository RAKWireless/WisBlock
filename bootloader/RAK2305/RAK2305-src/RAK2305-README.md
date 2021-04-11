# Compiling and flashing the project
Compiling the esp32-at is the same as compiling any other project based on the ESP-IDF:

1. You can clone the project into an empty directory using command:
```
git clone --recursive https://github.com/espressif/esp32-at.git
```
  * Note the `--recursive` option! The esp32-at project already has an ESP-IDF as a submodule, which should be used to compile the project, in order to avoid any compatibility issues (e.g. compile failure or running problems, etc.). 
  * If you have already cloned the esp32-at without this option, run another command to get all the submodules:
```shell
git submodule update --init --recursive
```  
2. `rm sdkconfig` to remove the old configuration.
3. Set the latest default configuration by `make defconfig`. 
4. `make menuconfig` -> `Serial flasher config` to configure the serial port for downloading.
5. `make flash` to compile the project and download it into the flash.
  * Or you can call `make` to compile it, and follow the printed instructions to download the bin files into flash by yourself.
  * `make print_flash_cmd` can be used to print the addresses of downloading.
  * More details are in the [esp-idf README](https://github.com/espressif/esp-idf/blob/master/README.md).
6. `make factory_bin` to combine factory bin, by default, the factory bin is 4MB flash size, DIO flash mode and 40MHz flash speed. If you want use this command, you must fisrt run `make print_flash_cmd | tail -n 1 > build/download.config` to generate `build/download.config`.
7. If the ESP32-AT bin fails to boot, and prints "ota data partition invalid", you should run `make erase_flash` to erase the entire flash.
