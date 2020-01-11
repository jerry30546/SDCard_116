NXP S32K116 with SD Card Module Demo
====================================

This is a S32DS project with S32K116 EVB. In this project, it shows some basic functions with SD Memory Card(SDMC), 
including HW initialization, erase, read, write block and show SDMC information.

## Background
Most of Microcontrollers don't have SDIO to access SDMC, but SDMC supports SPI mode to operate. So, this project 
would base on SPI mode to design program. And you can also reference to [Arduino SD Library](https://github.com/arduino-libraries/SD) for more details.

## Hardware Setup
The following figure is shown hardware setup for this project.
Due to SDMC operating voltage should under 3.6V, please confirm MCU voltage setting before insert SDMC.
And UART baudrate setting is 115200 in this project.
![HW_setup](https://user-images.githubusercontent.com/2669426/72203953-6c594e00-34ad-11ea-9601-d22cdf2cbf08.jpg)

## Software Setup
1. Clone project source code
```
git clone https://github.com/jerry30546/SDCard_116.git
```
2. Open NXP S32DS IDE, ***File -> Open Projects from File System... -> select project path -> Finish***
<img width="1280" alt="import_project" src="https://user-images.githubusercontent.com/2669426/72205545-53f22f00-34bf-11ea-92b4-e4e3e9ed930a.png">
3. Modify source code, which are generated from ProcessorExpert

*  Generated_Code/SPI0.c
*  Generated_Code/SPI0.h

In order to use dynamical SPI baudrate, we should remove ***const*** from LPSPI config variables.
<img width="1280" alt="const_remove" src="https://user-images.githubusercontent.com/2669426/72205544-53f22f00-34bf-11ea-9298-fd3b2fc5610f.png">

***If you re-generate ProcessorExpert code, please do this step again.***

## Execute Program
There are five SDMC features in this program, SDMC init, read block data, write data into block, erase block data, and show SDMC information.
Please use console(UART) interface to control SDMC features.
<img width="1280" alt="console_log" src="https://user-images.githubusercontent.com/2669426/72232856-b9006e80-35fe-11ea-8504-df30ab4bbf38.png">

