# ESP32 Smart Clock 
This is my Embedded System Design Semester Project. I am using the ESP32 platform to make a LCD clock that displays time synchronised from an NTP server and current weather forecasts from a HTTP based weather API. This repository contains all the subcomponents, design files, and source code for this system. It is designed to work out of the box. 

## Using this application
This application is created in the esp-idf framework. The toolchain, compiler, drivers, all can be installed locally here: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

This application was developed on a Windows system, and the windows installer: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html can be found here that automates the setup process. 
Upon installation and following these steps, a ESP_IDF 4.x CMD utility will be installed, which is a development environment for esp-idf based projects. Open this. I used version 4.4, but a newer version may be available upon completion of this project.
### Configure ESP-IDF
```console
install
export.bat
```
### Configure and build this project
```console
cd projects
git clone 
idf.py fullclean
idf.py build
```

### Configure Application settings
Some configurations may need to be made when using this application. Wifi SSID, Password, and authentication mode must be set before using this app, or the defaults will be used. Generally each component will have a configuration manager that is defined in components/component/Kconfig.Projbuild . Instead of editing hard-coded values in each component source file, a menuconfig utility is provided that can edit these values via command utility:
```console
idf.py menuconfig
```
* Menuconfig Main menu
<img width="464" alt="image" src="https://user-images.githubusercontent.com/68623356/198836527-44ae85d8-0757-4f0b-950e-2c7410b16ec1.png">

* Wifi Configuration 
<img width="463" alt="image" src="https://user-images.githubusercontent.com/68623356/198836554-73a397ea-e539-46e5-b199-f2d0223e2186.png">


## General Project Structure
```
├── CMakeLists.txt
├── components                 Developer created component sources
    ├──display_driver          Functions and resources to interact with an LCD dsiplay
    ├──net_ctlr                event handler, wifi config, web request functions for networking functions (May be split up into further subcomponents)
    ├──sys_resouce             Semaphores, queues, thread safe functions and types for task synchronization
├── main
│   ├── CMakeLists.txt
│   ├── component.mk    
│   |── Kconfig.Projbuild
│   └── Weather_Clock.c        Entry point to main application. 
├── test                       Unity based subproject for entering test application
│   ├── CMakeLists.txt     
│   ├── component.mk    
│   |── main                     
│   └── build
├── Makefile                   Makefile used by legacy GNU Make
└── README.md                  This is the file you are currently reading
```
## Testing
To verify that the system components work as designed, a /test directory is created that compiles into a testing environment based on the Unity framework for unit testing. Each component also has a /test subdirectory, ie. /components/net_ctlr/test that has defined Test cases with the unity macro: TEST_CASE("Test_name", "[args]")
