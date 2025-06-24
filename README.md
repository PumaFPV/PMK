# Puma Mechanical Keyboard
PMK - a full wireless HID ecosystem for split keyboards and other fun devices

The goal is to make a suit of wireless devices such as "smart keyboard" (combined mouse and keyboard), gamepad, LEDs, knobs, display, etc  
The first step is to make a wireless split keyboard, and then we will see later...

Currently, you can connect up to 8 (software limited) devices per dongle, each of them having up to 8 (software limited) layers. Layers only affect keyboards and LED.

## Dongle
The dongle is "master" ESP32S2/S3, appears as HID (keyboard, mouse and joystick) and as Mass Storage device Class with config files inside.
Maybe also store password manager later...

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

Exemple file structure can be found [here](https://github.com/PumaFPV/PMK/tree/main/Software/PMK_dongle/data/leftKB) 

For more details about devices configurations / capabilities check the [Protocol](https://github.com/PumaFPV/PMK?tab=readme-ov-file#protocols) chapter.

### Keyboard config file - HID codes
Check out the [PMK HID keyboard keycodes.md](https://github.com/PumaFPV/PMK/blob/main/PMK%20HID%20keyboard%20keycodes.md) for more information on which code corresponds to which key.


## Devices
Can run on any espressif chip that can run ESP-NOW. (I will later add support for nRF52 chips for lower power consumption, but it will be incompatible with ESP32 hardware)
The plan is to run the simplest firmware on all devices and being able to configure them via their corresponding .json files on the master dongle. 
Each device should be configurable via UART (set device ID, set dongle MAC address, and read device MAC address). 
See sample [device code](https://github.com/PumaFPV/PMK/tree/main/Software/PMK_device) for example. (Needs an update)

### Keyboard
For example a keyboard would send pressed keys number "0x04, 0xA1", which the dongle then reads in the .json as coordinates for returning desired key "a,b", if special key is pressed (volume up, brighntess control...) then do the appropriate consummer control action.
Keys are read with 74HC165 shift registers so theoratically you could have A LOT of keys per keyboard. Current limitation is the max value of keyID which is 255, so there could be a maximum of 255 keys per keyboard (+8 layers...).
For more information about the hardawre, check the [hardware](https://github.com/PumaFPV/PMK/tree/main?tab=readme-ov-file#hardware) section.
A casing is being designed, it is 3D printable. As foam grid is put inside for better sound (I think it is better?). 
It support module on the thumb side ([Circle trackpad](https://www.cirque.com/glidepoint-circle-trackpads) and [3D mouse](https://hackaday.io/project/187172-os3m-mouse)). Both modules have been tested and work. See [Space mouse](https://github.com/PumaFPV/PMK/tree/main?tab=readme-ov-file#space-mouse) section for more info.

<img src="/Layout/keyboard-layout.jpg" width="536" height="242">

### Mouse
Mouse can send x,y movement, wheel, pan scrolling and mouse buttons (left, middle, right, forward, backward). Each button is encoded as a bit in the "key" byte of the packet.
No configuration needed on the dongle appart from the DPI setting.
I plan on retro fitting a wired Logitech G300 with PMK so that it frees up a USB port + becomes wireless. It will be a great first POC. I also have a M330 laying around. It might also end up as a PMK mouse.

### Gamepad
Gamepad can send 2 analog joysticks, 2 analog triggers, dpad buttons, and 32 additionnal buttons. No additionnal configuration needed on the dongle.
I current upgraded an old thrustmaster sim set (FCS & WCS) from 1992 with PMK. They used to have an ADB port, but now both are fully wireless and appear as one gamepad on the computer. The update is a bit crude as it was first though as a POC for trying out the gamepad feature. But since it works fine, I kept it this way.

### LED
(For release 1.2.0)
Hopefully processing key press and led function on dongle and send data to device is fast enough, otherwise I will have to run led function on keyboard. (TODO)
Buildin keyboard functions: all kb, breath, swipe wave (single color or rainbow), key wave, single key... (TODO / Partially implemented...)
Uses the WS2812 kind of LED IC.

<img src="/Documentation/Images/PKB_HW00_pulsar.png" width="700" height="394">

### Knob
Knobs are in absolute position going from 0 to 255. Could be used for Deej (now only used for deej).

For encoder knobs, this is handled on the device, so it can enumerate as a keyboard pressing one key for clockwise and another for counterclockwise rotation. But it could also be treated as an absolute position depending on the device software.

### Space mouse
The software is mostly inspired from [Magellan](https://github.com/jfedor2/magellan-spacemouse/tree/master) and from reports analysed from a 3DConnexion Space mouse with [hid-tool](https://gitlab.freedesktop.org/libevdev/hid-tools). 
The current electronics and plastics hardware are from [OS3M mouse](https://hackaday.io/project/187172-os3m-mouse) and it works well. The software still needs some adjustements for calibration.
Note: If standard VIK connector is needed, PCB has to be flipped. No special hardware modifications are needed.

## Protocols

### ESP-NOW - Device to Dongle
This is the packet sent via ESP-NOW. 
Packet structure is: 
| Byte | 1 | 2 | 3-16 |
| ---- | - | - | - |
| Description | DeviceID | PacketID | Payload |

The goal of this packet strucure is that the dongle knows who is sending data and what kind of data to expect from the payload.
The deviceID is mostly used for keyboards and mouse to use the corresponding config files.

the payload's goal is to send data as simple as possible, e.g. keyboard sends key IDs pressed (max 8 at a time) and then the dongle processes the key ID depending on the json configuration

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
  - macaddress / getmacaddress : display dongle MAC address
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
- macaddress / getmacaddress : returns device MAC address
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
Dongle is still HW00 (first rev), it needs 3 small patches to work properly. A [HW01](https://github.com/PumaFPV/PMK/tree/main/Hardware/PMK_Dongle/PMK_Dongle_HW01) is designed but hasn't been ordered yet.
The ESP32S2 has some limitations, so a ESP32S3 dongle is being made too [here](https://github.com/PumaFPV/PMK/tree/main/Hardware/PMK_Dongle/PMK_Dongle_S3_HW00).

Even a nRF52 dongle might be made... Who knows[?](https://github.com/PumaFPV/PMK/tree/main/Hardware/PMK_Dongle/PMK_Dongle_C_nRF_HW00)

### The keyboard
As of today the keyboard is at the HW02 stage, and uses the sofle layout (which I do like). It uses 29 hot swappable Kailh Choc switches + one rotary encoder. The MCU is a [PSoM module](https://github.com/PumaFPV/PSoM) (homemade SoM) with an ESP32S2 inside. The SoM allows for quick swap of different MCU to try out different feature. Today the ESP32S2 is used for the ease of software developpement. Its power consumption is bad I am aware. But at least software works.
Here is the changelog.

| HW rev | change list |
| ------ | ----------- |
| HW00   | Based on moonlander layout, integrated ESP32, used as a proof of concept, didn't like the layout in the end |
| HW01   | Based on Sofle, PSoM connector (swappable uC for futur proofing), added rotary encoder, way better layout in my opinion. Added side connector option for trackpad or space mouse |
| HW02   | Still soffle, added support for kailh choc V2 (failed), moved the rotary encoder up, added debouncing for rotary encoder, fixed the side module connector, moved test point to accessible location, updated PSoM pinout for better compatibility |
| HW03 (WIP)  | Still soffle, fixed the silkscreen, Added slot for dongle, fixed V2 footprint, magnetic pogo module? |

More info on [Notion](https://swamp-zydeco-907.notion.site/PumaKeyBoard-b41d42fec8c74b02bc73637fae3648d7)

### Edit the PCBs
If you want to make your own keyboard, or visualize current dongle, keyboard or other devices you will need KiCAD 8 and Component_lib which is my own components library. It can be found in its [own repo](https://github.com/PumaFPV/Component_lib), information on own to install it can be found there.

# Building 
This whole project is built using PlatformIO on VS Code / VS Codium. (If using VS Codium note that the marketplace for extensions doesn't really work, you will need to download cpp and platformIO .vsix extension files from https://marketplace.visualstudio.com/vscode)

Once this is installed you can clone PMK's repo ````git clone https://github.com/PumaFPV/PMK````
and open whichever code you want to build / modify: ````PMK/Software/PMK_keyboard```` with platformIO "Open project".

Once project is opened and loaded (might take a few minutes for the first time) (it needs internet connection to download library and toolchain) you can compile with ````ctrl + shift + B```` or ````ctrl + shift + U```` to compile and upload, or use the bottom ribbon (make sure to select the correct workspace before compiling and uploading). 