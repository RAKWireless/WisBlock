@echo off

@echo ######################################################
@echo #                                                    #
@echo #             WisBlock Download Tool                 #
@echo #                 Version: V1.0.1                    #
@echo #                                                    #				
@echo ######################################################



@echo Choose download mode.
@echo Press Enter to confirm.
@echo A: Upgrade App.
@echo B: Upgrade BootLoader.

set /p MODE=
if "%MODE%"=="A" goto A
if "%MODE%"=="B" goto B
:A
@echo Download port:
set /p DownloadPort=
@echo Download file name:
set /p DownloadFileName=
.\Tools\adafruit-nrfutil.exe --verbose dfu serial -pkg .\%DownloadFileName% -p COM%DownloadPort% -b 115200 --singlebank
goto C

:B
@echo Download port:
set /p DownloadPort=
@echo Download file name:
set /p DownloadFileName=
.\Tools\adafruit-nrfutil.exe --verbose dfu serial --package .\%DownloadFileName% --port COM%DownloadPort% -b 115200 --singlebank --touch 1200
goto C

:C
@echo Download completed.
pause

