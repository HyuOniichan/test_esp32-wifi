# ESP WIFI MODULE

## Description
ESP32 WiFi management: STA, AP, Flash config, MAC spoofing.

## Environment
+ IDE: Visual Studio Code + Platform IO (Arduino Framework)
+ Architecture: 
  + ESP32 DevKit-V1

## Features
+ WiFi Station mode (STA) & Access Point mode (AP).
+ Persistent configuration (Flash memory).
+ MAC address spoofing (STA mode).
+ Web-based configuration portal (AP mode).

## Workflow
```mermaid
graph TD
    %% All
    loadConfig[Load config from\n Flash memory]
    whichMode{WiFi mode?}

    %% apMode
    webServer[Start web server]
    saveUserConfig["Save user config to Flash\n (wifi mode, ssid, pass)"]
    hardRestart["Hard restart ESP\n (EN button)"]

    %% staMode
    saveCredentials["Get ssid, pass\n from config"]
    macSpoofing[MAC Spoofing]
    connectWifi[Connect to WiFi]

    %% Visualize
    loadConfig --> whichMode
    whichMode --> apMode
    whichMode --> staMode

    subgraph apMode["AP Mode"]
        direction TD 
        webServer
            -- POST --> saveUserConfig
            --> hardRestart
    end

    subgraph staMode["STA Mode"]
        direction TD 
        saveCredentials
            --> macSpoofing
            --> connectWifi
    end
```

## References
[1] [neittien0110/FineDustMonitor](https://github.com/neittien0110/FineDustMonitor)  
[2] [neittien0110/siotcore_sdk_v2](https://github.com/neittien0110/siotcore_sdk_v2)  
