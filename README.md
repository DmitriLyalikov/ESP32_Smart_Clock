# ESP32 Smart Clock 
This is my Embedded System Design Semester Project. I am using the ESP32 platform to make a LCD clock that displays time synchronised from an NTP server and current weather forecasts from a HTTP based weather API. This repository contains all the subcomponents, design files, and source code for this system. It is designed to work out of the box. 
### Demonstration Hardware 
* ESP32-DevKitC-V4 (ESP32 Based evaluation board)
* LCD1602          ( 2x16 LCD)
* PCF8574 I2C Adaptor ( LCD I2C Controller) 

### Live Weather 
Weather information like Precipitation, Clouds, Pressure, Temperature, Wind are accessed for the desired City or GeoLocation via the  https://openweathermap.org/current HTTP server. This has an API which processes HTTP requests and returns a JSON response including the weather data. 

#### API interaction
An API key is required to acceess the API. It is also appended into the query string of HTTP requests. To get a new API key for a new device, register at https://openweathermap.org/api.  

![image](https://user-images.githubusercontent.com/68623356/198903311-52917fbb-a0ba-43ab-8318-dec37035fe93.png)

To verify this key works correctly, wait 10 minutes since activation and request this URL in a browser:
```console
http://api.openweathermap.org/data/2.5/weather?q=Lisbon,pt&APPID={yourAPIkey}
```

The base endpoint is: 
```console 
api.openweathermap.org/data/2.5/weather
```
A query parameter is passed that includes the City, Country code following 
ISO 31666 Country codes (https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes. 
In our case for New York City, USA the q-parameter is: 
```console
q=NY,US
```

With both an API key, and city, a complete query will appear as: 
```console
http://api.openweathermap.org/data/2.5/weather?q={City},pt&APPID={yourAPIkey}
```

### Using this application
This application is created in the esp-idf framework. The toolchain, compiler, drivers, all can be installed locally here: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/

This application was developed on a Windows system, and the windows installer: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html can be found here that automates the setup process. 
Upon installation and following these steps, a ESP_IDF 4.x CMD utility will be installed, which is a development environment for esp-idf based projects. Open this. I used version 4.4, but a newer version may be available upon completion of this project.
#### Configure ESP-IDF
```console
install
export.bat
```
#### Configure and build this project
```console
cd projects
git clone https://github.com/DmitriLyalikov/ESP32_Smart_Clock.git
cd ESP32_Smart_Clock
idf.py fullclean
idf.py set-target ESP32
idf.py build
```

#### Flashing and Running
With the ESP32 connected via USB, connected to 'COMx'
```
idf.py flash monitor 'COMx'
```
This will flash the application onto the system, and restart, it will then provide serial access to the application. This is not needed to use the clock, but is useful to ensure it flashed successfully. 

#### Configure Application settings
Some configurations may need to be made when using this application. Wifi SSID, Password, and authentication mode must be set before using this app, or the defaults will be used. Generally each component will have a configuration manager that is defined in components/component/Kconfig.Projbuild . Instead of editing hard-coded values in each component source file, a menuconfig utility is provided that can edit these values via command utility:
```console
idf.py menuconfig
```
* Menuconfig Main menu
<img width="464" alt="image" src="https://user-images.githubusercontent.com/68623356/198836527-44ae85d8-0757-4f0b-950e-2c7410b16ec1.png">

* Wifi Configuration 
<img width="463" alt="image" src="https://user-images.githubusercontent.com/68623356/198836554-73a397ea-e539-46e5-b199-f2d0223e2186.png">


### General Project Structure
```
├── CMakeLists.txt
├── components                 Developer created component sources
    ├──i2c-lcd1602             Functions and resources to interact with an LCD display. 3rd party library (will not be tested)
    ├──esp32-smbus             smbus/i2c driver that lcd requires. 3rd party library (will not be tested)
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

### Component Structure
Each component contains an include directory where the component_name.h is kept, and a test subdirectory where unit tests are defined for this component.
As discussed before, each component also has a KConfig.Projbuild that is used to configure some settings like pin assignments, clock rates, etc. 
```
├── CMakeLists.txt
├── component.mk
├── component_name.c
├── KConfig.Projbuild
├── include              
    ├──component_name.h                   
├── test
│   ├── CMakeLists.txt
│   ├── component.mk    
│   └── test_component_name.c  
```
This structure allows a clear dependency tree for our entry point at Weather_Clock.c in /main. All functions, definitions and types are brought into the main namespace by including the header file in our application. It is important to note that this inclusion also brings the components dependencies into scope as well. For example, i2c-lcd1602 component depends on the esp32-smbus component for its i2c driver, and by using #include "i2c-lcd1602.h" in Weather_Clock.c,
esp32-smbus has also been included, by association. 
### Testing
To verify that the system components work as designed, a /test directory is created that compiles a test binary based on the Unity framework for unit testing. Each component also has a /test subdirectory, ie. /components/net_ctlr/test that has defined Test cases with the unity macro: TEST_CASE("Test_name", "[args]")
