# PumaMechaniclKeyboard
PMK - a Full wireless HID ecosystem for split keyboards and other fun devices

The goal is to make a suit of wireless devices such as "smart keyboard" (combined mouse and keyboard), LEDs, knobs, display, etc  
The first step is to make a wireless split keyboard, and then we will see later...

## Dongle
The dongle is "master" ESP32S2/S3, appears as HID (keyboard, mouse and joystick) and as Mass Storage device Class with config files inside.
Maybe also store password manager....

<img src="/Documentation/Images/Dongle_HW00_recto.jpg" width="231" height="225"><img src="/Documentation/Images/Dongle_HW00_verso.jpg" width="255" height="195"><img src="/Documentation/Images/Dongle_HW00_PCB_bottom.jpg" width="225" height="300">

As the dongle is the master of PMK, it hosts the config files for each devices. Here is the expected file organisation:

		|--deviceName1  
		|  |- deviceName1.json  
		|  |--keyboard  
		|  |  |- kb-l1.json  
		|  |  |- kb-l2.json  
		|  |--led  
		|  |  |- led-l1.json  
		|  |  |- led-l2.json  
		|--deviceName2  
		|  |- deviceName2.json  
		|  |--keyboard  
		|  |  |- kb-l1.json  
		|  |  |- kb-l2.json  

		
For more details about devices configurations / capabilities check the Protocol chapter.

## Devices
Can run on any espressif chip that can run ESP-NOW. (Will later add support for nRF52 chips for lower power consumption)
The plan is to run the simplest firmware on all devices and being able to configure them via their corresponding .json files on the master dongle. 
Each device should be configurable via UART (set device ID, set dongle MAC address, and read device MAC address). 
See sample device code for example. (TODO)

### Keyboard
For example a keyboard would send pressed keys number "0x04, 0xA1", which the dongle then reads in the .json as coordinates for returning desired key "a,b" pr modifier key., if special key is pressed (volume up, brighntess control...) then do the appropriate consummer control action.

<img src="/Layout/keyboard-layout.jpg" width="536" height="242">

### Mouse
Mouse can send x,y movement, wheel, pan scrolling and mouse buttons (left, middle, right, forward, backward). Each button is encoded as a bit in the "key" byte of the packet.
No configuration needed on the dongle appart from the DPI setting.

### Gamepad
Gamepad can send 2 analog joysticks, 2 analog triggers, dpad buttons, and 32 additionnal buttons. No additionnal configuration needed on the dongle.

### LED
Hopefully processing key press and led function on dongle and send data to device is fast enough, otherwise we will have to run led function on keyboard. (TODO)
Buildin keyboard functions: all kb, breath, swipe wave (single color or rainbow), key wave, single key... (TODO / Partially implemented...)

<img src="/Documentation/Images/PKB_HW00_pulsar.png" width="700" height="394">

### Knob
Knobs are in absolute position going from 0 to 255. Could be used for Deej.

For encoder knobs, this is handled on the device and enumerates as a keyboard pressing one key for clockwise and another for counterclockwise rotation.


## Protocols

### ESP-NOW - Device to Dongle
This is the packet sent via ESP-NOW. 
Packet structure is: 
| Byte | 1 | 2 | 3-16 |
| ---- | - | - | - |
| Description | DeviceID | PacketID | Payload |

Devices should send data as simple as possible, keyboard sends key IDs pressed (max 8 at a time) and then the dongle processes the key ID depending on the json configuration

| Packet ID | Packet type | Packet information | Number of bytes | Example packet | Example description |
| --------- | ----------- | ------------------ | --------------- | -------------- | ------------------- |
| 0x00 | Telemetry | Send battery voltage, temperature, mac address, errors... | 15 | dID 00 battery temperature address[6] err err err err err |  |
| 0x01 | Keyboard | 1 byte = 1 key ID | 8 | dID 01 A2 34 DB 32 21 4F 00 00 | 6 keys are pressed |
| 0x02 | Mouse | move x, move y, wheel, pan, mouse clicks | 7 | dID 02 xx yy ww pp kk |move x relative, move y relative, move w relative, move p relative, kk key press|
| 0x03 | Gamepad | 2 joysticks, 2 analog triggers, 9 directions d pad, 32 buttons | 12 | dID 03 xx yy zz rz rx ry hh bb bb bb bb bb |  |
| 0x04 | LED | Choose pre defined function, and R, G and B | 8 | dID 04 func ledNumberStart ledNumberEnd Red Green Blue |  |
| 0x05 | Knob / Rotary encoder / potentiometer | each byte is a pot value, max 8 pot per MCU | 10 | dID 05 FF 00 FF 00 FF 00 FF 00 | 4 pots are at max, 4 are at min |
| 0x06 | Actuator | Choose command type (torque, speed, position) | 5 | dID 06 01 |  |
| 0x07 | Display | send image to display using wifi for faster refresh rate? or store on local mem | 6 | dID 07 imgNumber x y brightness | Display img number n at coordinates x,y and brightness b. Might change later |
| 0x08 | Serial | send up to 8 bytes of data | 10 | dID 08 aa bb cc dd ee ff gg hh | Sending 8 bytes of data |
| 0x09 |  |  |  |  |  |

### Serial config - Dongle to PC
The dongle supports a few serial commands, they only consist on a string, no CR/LF should be send or the command won't be recongnised.
Here is the list of supported commands:
- getMacAddress : Returns the dongle MAC address
- cpu : Returns data about CPU (frequencies, flash size...)
- version : Returns firmware version and build date
- power : Return WiFi power output in ESP manner
- format : Format flash in FatFS format
- restart : Restarts the dongle
- l-1 / l7 : Forces current layer to layer x as specified by lx. Send l-1 to turn off force layer feature (keypress change layer will reset the force layer)

### Serial config - Device to PC (TODO)
Before being able to communicate with the dongle, the device has to know the dongle MAC address, and must own a deviceID. You will also need to know the device MAC address to give it to the dongle.
Here is the list of supported commands:
- getMacAddress : Returns the device MAC address
- setDongleMacAddress + dongle MAC address : Set the dongle MAC address in device memory 
- setDeviceID + deviceID : Give the device its ID used to link to configuration file in the dongle

More info on [Notion](https://swamp-zydeco-907.notion.site/PumaKeyBoard-b41d42fec8c74b02bc73637fae3648d7)
