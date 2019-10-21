---
Area: Tutorial
TOCTitle: Sigfox Arduino Shield Demo
PageTitle: Working with Cytron Sigfox Arduino Shield
---


# Working with Cytron Sigfox Arduino Shield

<strong>Cytron Sigfox Arduino Shield Setup</strong>
<br/><br/>
Controller: <a href="https://my.cytron.io/p-maker-uno-simplifying-arduino-for-education" target="_blank">Maker UNO (Arduino UNO compatible)</a><br/>
IDE: <a href="https://www.arduino.cc/en/Main/Software" target="_blank">Arduino IDE </a><br/>

![alt text](https://raw.githubusercontent.com/wemakerspace/Cytron_Sigfox_Demo/master/sigfoxshield-v2s.png "Cytron Sigfox Shield")



| Arduino Pin | Sigfox Shield |           Description           |
|:-----------:|:-------------:|:-------------------------------:|
|    IOREF    |       5V      |   5V Power Source from Arduino  |
|    RESET    |     RESET     |      Reset MCU, Low Active      |
|     3V3     |      3V3      |  3V3 Power Source from Arduino  |
|      5V     |       5V      |   5V Power Source from Arduino  |
|     GND     |      GND      |               GND               |
|     VIN     |       NC      |                NC               |
|      A0     |       NC      |           Analog Pin-0          |
|      A1     |       NC      |           Analog Pin-1          |
|      A2     |       A2      |      Analog Pin-2; Grove #2     |
|      A3     |       A3      |      Analog Pin-3; Grove #2     |
|      A4     |    I2C-SDA    |      Analog Pin-4; Grove #1     |
|      A5     |    I2C-SCL    |      Analog Pin-5; Grove #1     |
|     D13     |       NC      |          Digital Pin-13         |
|     D12     |       NC      |          Digital Pin-12         |
|     D11     |       NC      |          Digital Pin-11         |
|     D10     |       NC      |          Digital Pin-10         |
|      D9     |    Red LED    |      User Programmable LED      |
|      D8     |   Green LED   |      User Programmable LED      |
|      D7     |     Wakeup    | Wakeup Pin from deep sleep mode |
|      D6     |     Button    |   Push button, Trigger to low   |
|      D5     |    UART-RX    |        UART Receiving Pin       |
|      D4     |    UART-TX    |      UART Transmitting Pin      |
|      D3     |      INT2     |  Interrupt#2 for Accelerometer  |
|      D2     |      INT1     |  Interrupt#1 for Accelerometer  |
|      D1     |       NC      |          Digital Pin-1          |
|      D0     |       NC      |          Digital Pin-0          |

<strong>I2C Sensor Address</strong>
- SM303AGR Accelerometer	`0x32`
- SM303AGR Magnet `0x3C`
- BOSCH Temperature / Humidity / Pressure Sensor – BME280 `0x76`

How to install it:
==================

Arduino Library: <a href="https://raw.githubusercontent.com/wemakerspace/Cytron_Sigfox_Demo/master/unabiz-arduino-master.zip" target="_blank">Sigfox Library </a><br/>
#### THIS LIBRARY IS KEPT FOR REFERENCE. THE OFFICIAL LIBRARY IS HOSTED AT : https://github.com/UnaBiz/unabiz-arduino  ####





- Download the library from GitHub : https://raw.githubusercontent.com/wemakerspace/Cytron_Sigfox_Demo/master/unabiz-arduino-master.zip

## Installation
1. Open the Arduino IDE, select `Sketch` -> `Include Library` -> `Add.ZIP Library`.
2. Select the zip file you just download.
3. Open the Arduino IDE, select `Sketch` -> `Include Library` -> `Manage libraries...`.
4. Search for `Adafruit BME280 Library` and `Adafruit Unified Sensor`
3. Click `Install` to install the library.
4. Restart the Arduino IDE.
5. Examples can be opened in Arduino IDE from `File` -> `Examples` -> `Unabiz-arduino`->`Cytron Sigfox BME280`.<br>


Setup SigFox Backend
====================
## Device Enrollment
1. Go to [Sigfox Activation Platform](https://backend.sigfox.com/activate)
2. Compile and run the `Cytron Sigfox BME280` example in your arduino + Shield
3. Click `Serial Monitor` to see the serial output.
4. Copy the `SIGFOX ID = XXXXXXXX` & `PAC = XXXXXXXXXXXXXXX` information
5. Register the device to your Sigfox Account.<br>

![alt text](https://raw.githubusercontent.com/wemakerspace/Cytron_Sigfox_Demo/master/Arduino-serial-output.png "Arduino Serial Output")

## Setup ThingSpeak
1. Register a free account in [ThingSpeak](https://thingspeak.com/)
2. We need to create 2 Channels:
    - 1st Channel for Sigfox backend to pipe encoded data
    - 2nd Channel for ThingSpeak to process and display decoded data.
3. Create both channel with detail as per below:
    - Field1 : `Temperature`
    - Field2 : `Pressure`
    - Field3 : `Humidity`
4. Once channel is created, copy both channel API key to be use later.

![alt text](https://raw.githubusercontent.com/wemakerspace/Cytron_Sigfox_Demo/master/thingspeak-channel.png "Thingspeak Channel")

## Setup Sigfox Callback
1. Go to [Sigfox Backend](https://backend.sigfox.com/)
2. `DEVICE TYPE`->`LIST`->right click on device and select `EDIT`
3. Select `CALLBACKS` at the left side bar
4. Select `New` on the top right
5. Select `Custom CallBack`
    Please input the data as per below:
    - TYPE: `DATA/UPLINK`
    - Channel: `URL`
    - HTTP Method: `GET`
    - Custom Payload: 
```javascript
temp::int:16:little-endian pres::uint:16:little-endian humi::uint:16:little-endian
```
- URL Pattern:
```javascript
https://api.thingspeak.com/update?api_key=XXXXXXXXXXXXXXX&field1={customData#temp}&field2={customData#pres}&field3={customData#humi}
```

The api_key will need to be replace by your thingspeak channel Write API key.(Channel 1 Write API Key)

![alt text](https://raw.githubusercontent.com/wemakerspace/Cytron_Sigfox_Demo/master/Sigfox-backend-callback.png "Sigfox Callback")

# Sigfox Messages
Since the Sigfox network can send a maximum of 140 messages per day, we'll optimize the readings and send data in compact binary format.

Data packing is important because the maximum size of a message you can send is 12 bytes. A float occupies 4 bytes, so we'll need to pack our readings into some smaller representation.

The float format is very wide (you can represent numbers as big as 3.402823e+38 (!) ) and we don't need the whole data space; we can then decide safe limits (based on where we are going to deploy the station) and restore the original representation in the backend.

![alt text](https://raw.githubusercontent.com/wemakerspace/Cytron_Sigfox_Demo/master/Sigfox-backend-message.png "Sigfox Message")

## Decoding 12byte Sigfox Payload

1. Go to [MATLAB Analysis](https://thingspeak.com/apps/matlab_analyses)
2. Click `New`
3. Enter the following code:
```matlab
% TODO - Replace the [] with channel ID to read data from: 
readChannelID = XXXXXX; 
% TODO - Enter the Read API Key between the '' below: 
readAPIKey = 'XXXXXXXX';  
% TODO - Replace the [] with channel ID to write data to: 
writeChannelID = XXXXXX; 
% TODO - Enter the Write API Key between the '' below: 
writeAPIKey = 'XXXXXXXXXX';  
%% Read Data %% 
data = thingSpeakRead(readChannelID, 'ReadKey', readAPIKey,'OutputFormat','table');  
analyzedData = data;  
%% Analyze Data %% 
INT16_t_MAX = 32767; 
UINT16_t_MAX = 65536; 
analyzedData.('Temperature') = data.('Temperature') / INT16_t_MAX * 120 ; 
analyzedData.('Pressure') = data.('Pressure') / UINT16_t_MAX * 200000 ; 
analyzedData.('Humidity') = data.('Humidity') / UINT16_t_MAX * 110 ; 
%% Write Data %% 
thingSpeakWrite(writeChannelID, analyzedData, 'WriteKey', writeAPIKey); 
%% Schedule action: React -> every 10 minutes
```
4. Press `SAVE`
5. Scroll down the `Schedule Actions` -> `React`
6. Create a new `React` with setting below:
    - Condition Type : `No Data Check`
    - Test Frequency : `Every 10 minutes`
    - Condition : `if channel have no been updated for 1 minutes`
    - Action : `MATLAB Analysis Code to Execute`
    - Option : ` Run action each time conditon is met`
7. Save.

# ThingSpeak Dashboard

Once you done all this, the data from the Arduino will be send over to Sigfox server and then forward to Thingspeak every 10 minute.

![alt text][logo]

[logo]: https://raw.githubusercontent.com/wemakerspace/Cytron_Sigfox_Demo/master/thingspeak-dashboard.png "Thingspeak Dashboard"





Welcome to our <a href="http://forum.cytron.com.my/" target="_blank">technical forum</a> for futher inquiry.
