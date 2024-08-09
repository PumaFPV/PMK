# Puma Mechanical Keyboard
PMK - a Full wireless HID ecosystem for split keyboards and other fun devices

The goal is to make a suit of wireless devices such as "smart keyboard" (combined mouse and keyboard), LEDs, knobs, display, etc  
The first step is to make a wireless split keyboard, and then we will see later...

Currently, you can connect up to 8 (software limited) devices per dongle, each of them having up to 8 (software limited) layers. Layers only affect keyboards and LED.

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
Keys are read with 74HC165 shift registers so theoratically you could have A LOT of keys per keyboard. Current limitation is the max value of keyID which is 255, so there could be a maximum of 255 keys per keyboard (+8 layers...).
As of today the keyboard is at the HW01 stage, and uses the sofle layout (which I do like). It uses 29 hot swappable Kailh Choc switches + one rotary encoder. The MCU is a PSoM module (homemade SoM) with an ESP32S2 inside. The SoM allows for quick swap of different MCU to try out different feature. Today the ESP32S2 is used for the ease of software developpement. Its power consumption is bad I am aware. But at least software works. I plan to design a casing + metal grid + foam for it. 
HW02 will be coming soon too as I have a few adjustements to make. It support module on the thumb side (Circle trackpad and 3D mouse will be the first modules to come).

<img src="/Layout/keyboard-layout.jpg" width="536" height="242">

### Mouse
Mouse can send x,y movement, wheel, pan scrolling and mouse buttons (left, middle, right, forward, backward). Each button is encoded as a bit in the "key" byte of the packet.
No configuration needed on the dongle appart from the DPI setting.
I plan on retro fitting a wired Logitech G300 with PMK so that it frees up a USB port + becomes wireless. It will be a great first POC.

### Gamepad
Gamepad can send 2 analog joysticks, 2 analog triggers, dpad buttons, and 32 additionnal buttons. No additionnal configuration needed on the dongle.
I current upgraded an old thrustmaster sim set (FCS & WCS) from 1992 with PMK. They used to have an ADB port, but now are fully wireless and appears as one gamepad on the computer. The update is a bit crude as it was first though as a POC for trying out the gamepad feature. But since it works fine, I kept it this way.

### LED
Hopefully processing key press and led function on dongle and send data to device is fast enough, otherwise we will have to run led function on keyboard. (TODO)
Buildin keyboard functions: all kb, breath, swipe wave (single color or rainbow), key wave, single key... (TODO / Partially implemented...)
Uses the WS2812 kind of LED IC.

<img src="/Documentation/Images/PKB_HW00_pulsar.png" width="700" height="394">

### Knob
Knobs are in absolute position going from 0 to 255. Could be used for Deej.

For encoder knobs, this is handled on the device and enumerates as a keyboard pressing one key for clockwise and another for counterclockwise rotation.

### Space mouse
Mostly inspired from [Magellan](https://github.com/jfedor2/magellan-spacemouse/tree/master) and from reports analysed from a 3DConnexion Space mouse with [hid-tool](https://gitlab.freedesktop.org/libevdev/hid-tools) 

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
| 0x09 | SpaceMouse | send 3 bytes for translation (x, y, z) and 3 bytes for rotation (x, y, z), signed intergers | 6 | dID 7F 00 00 FF 00 00 | Positive translation on x and negative rotation on x |

### Serial config - Dongle to PC
The dongle supports a few serial commands, they only consist of a string, no CR/LF should be send or the command won't be recongnised. (The received command is hashed into a number then switched cased to the correct function)
Here is the list of supported commands:
  - h / help : display available commands
  - info : display all info
  - macaddress : display dongle MAC address
  - version : display build date and version
  - power : display RF Tx power
  - cpu : display cpu info
  - format : format file system
  - restart / reboot : restart the dongle
  - deej : toggle on / off deej
  - config / load: Reload configuration files
  - l0 / l1 / l2... : Force the current layer to... If one keybind moves to new layer it will be ignored use l-1 command to reset force layer
  - l-1 : disable forced layer
  - debug1 / debug2... : Toggle debug output. Different kind of debug available


### Serial config - Device to PC
Before being able to communicate with the dongle, the device has to know the dongle MAC address, and must own a deviceID. You will also need to know the device MAC address to give it to the dongle.
Here is the list of supported commands:
- h / help : returns available commands
- info : display all info
- macaddress : returns device MAC address
- setdonglemacaddress : update dongle MAC address, be sure to send values in HEX mode
- getdonglemacaddress : returns the current dongle MAC address
- setdeviceid : update deviceID to new value
- getdeviceid : returns currently set devi
- version : returns build date and version
- power : returns RF Tx power
- cpu : returns CPU info
- setcpufreq : set new CPU frequency
- restart : restart the dongle
- scan : Scan for side module presence
- debug1 / debug2... : Toggle debug output. Different kind of debug available


## Hardware
### The dongle
Dongle is still HW00 (first rev), it needs 3 small patches to work properly. A HW01 might be designed in the coming months.

Even a nRF52 dongle might be made... Who knows?

### The keyboard
Latest keyboard is HW02. 
Here is the changelog.

| HW rev | change list |
| ------ | ----------- |
| HW00   | Based on moonlander layout, integrated ESP32, used as a proof of concept, didn't like the layout in the end |
| HW01   | Based on Sofle, PSoM connector (swappable uC for futur proofing), added rotary encoder, way better layout in my opinion. Added side connector option for trackpad or space mouse |
| HW02   | Upgraded version of HW01, added support for kailh choc V2, moved the rotary encoder up, added debouncing for rotary encoder, fixed the side module connector, moved test point to accessible location, updated PSoM pinout for better compatibility |
| HW03 (WIP)  | Fixed the silkscreen, Added slot for dongle |

More info on [Notion](https://swamp-zydeco-907.notion.site/PumaKeyBoard-b41d42fec8c74b02bc73637fae3648d7)

# Building 
This whole project is built using PlatformIO on VS Code / VS Codium. (If using VS Codium note that the marketplace for extensions doesn't really work, you will need to download cpp and platformIO .vsix extension files from https://marketplace.visualstudio.com/vscode)

Once this is installed you can clone PMK's repo ````git clone https://github.com/PumaFPV/PMK````
and open whichever code you want to build / modify: ````PMK/Software/PMK_keyboard```` with platformIO "Open project".

Once project is opened and loaded (might take a few minutes for the first time) (it needs internet connection to download library and toolchain) you can compile with ````ctrl + shift + B```` or ````ctrl + shift + U```` to compile and upload. 