# PumaKeyBoard
 PKB - Full wireless split keyboard and other fun devices

The goal is to make a suit of wireless devices such as "smart keyboard" (combined mouse and keyboard), LEDs, knobs, display, etc
/n The first step is to make a wireless split keyboard, for the rest we will see later.

## Dongle
The dongle is master ESP32S2, appears as HID (keyboard, mouse and joystick) msc with right.json, left.json to store and update configuration 
password manager

## Devices
Can run on ESP32S2 or S3, but C3 doesnt bring advantages

### Keyboard
Same firmware on both sides, set device ID via UART, Dongle macAddress also.
PKB sends pressed keys id  “0xA0” (max 8), dongle receives id of pressed keys “0xA0”, corresponding to array address which then returns desired key “a”, if special key is pressed (shift, Fn, …) then use 2nd line of array “A”
![splitkeyboard](/Layout/keyboard-layout.jpg)

### LED
hopefully processing key press and led function on dongle and send data to kb is fast enough, otherwise we will have to run led function on keyboard.

## Protocol
Devices should send data as simple as possible, keyboard sends key IDs pressed (max 8 at a time) and then the dongle processes the key ID depending on the json configuration

| Packet ID | Packet type | Packet information | Number of bytes | Example packet | Example description |
| --------- | ----------- | ------------------ | --------------- | -------------- | ------------------- |
| 0x01 | Keyboard | 1 byte = 1 key ID | 8 | 01 A2 34 DB 32 21 4F 00 00 | 6 keys are pressed |
| 0x02 | Mouse | move x, move y, wheel, pan, mouse clicks |  | 02 xx yy ww pp kk |move x relative, move y relative, move w relative, move p relative, kk key press|
| 0x03 | Gamepad |  | 12 | 03 xx yy zz rz rx ry hh bb bb bb bb bb |  |
| 0x04 | LED |  | TBD |  |  |
| 0x05 | Knob / Rotary encoder / potentiometer | each byte is a pot value, max 8 pot per MCU | 8 | 05 FF 00 FF 00 FF 00 FF 00 | 4 pots are at max, 4 are at min |
| 0x06 | Display | send image to display use wifi for faster refresh rate? |  |  |  |
| 0x07 | Actuator |  |  |  |  |
| 0x08 | Telemetry | battery voltage, temperature, mac address, error |  |  |  |
| 0x09 | Serial |  |  |  |  |

More info on [notion](https://swamp-zydeco-907.notion.site/PumaKeyBoard-b41d42fec8c74b02bc73637fae3648d7)
