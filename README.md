# ESP32 Smart Clock 
This is my Embedded System Design Semester Project. I am using the ESP32 platform to make a LCD clock that displays time synchronised from an NTP server and current weather forecasts from a HTTP based weather API. This repository contains all the subcomponents, design files, and source code for this system. It is designed to work out of the box. 



## Testing
To verify that the system components work as designed, a /test directory is created that compiles into a testing environment based on the Unity framework for unit testing. Each component also has a /test subdirectory, ie. /components/net_ctlr/test that has defined Test cases with the unity macro: TEST_CASE("Test_name", "[args]")
