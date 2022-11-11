# PumaKeyBoard
 PKB - Full wireless split keyboard and other fun devices

The goal is to make a suit of wireless devices such as "smart keyboard" (combined mouse and keyboard), LEDs, knobs, display, etc
/n The first step is to make a wireless split keyboard, for the rest we will see later.

Most up to date informations are on [notion](https://swamp-zydeco-907.notion.site/PumaKeyBoard-b41d42fec8c74b02bc73637fae3648d7)

## Dongle
The dongle is master ESP32S2, every other device should be as simple as possible ESP32C3, only sends and receive raw data
Appear as msc with right.json,  left.json to store and update configuration 
password manager

## Keyboard
Same firmware on both sides : use resistor or something to differentiate
PKB sends pressed keys id  “0xA0” (max 8), dongle receives id of pressed keys “0xA0”, corresponding to array address which then returns desired key “a”, if special key is pressed (shift, Fn, …) then use 2nd line of array “A”

## LED
hopefully processing key press and led function on dongle and send data to kb is fast enough, otherwise we will have to run led function on keyboard.

## Protocol

| Packet ID | Packet type | Packet information | Number of bytes | Example packet | Example description |
| --------- | ----------- | ------------------ | --------------- | -------------- | ------------------- |
| 0x01 | keyboard | 1 byte = 1 key ID | 8 | 01 A2 34 DB 32 21 4F 00 00 | 6 keys are pressed |
| 0x02 | keypad / macro pad | 1 byte = 1 key ID | 8 | 02 A2 34 DB 32 21 4F 7A 8E | 8 keys are pressed |
| 0x03 | mouse | move x, move y, wheel, pan |  |  |move x relative move y relative move w relative move p relative|
| 0x04 | Gamepad |  | 12 | 04 xx yy zz rz rx ry hh bb bb bb bb bb |  |
| 0x05 | LED |  | TBD |  |  |
| 0x06 | Knob / Rotary encoder / potentiometer | each byte is a pot value, max 8 pot per MCU | 8 | 06 FF 00 FF 00 FF 00 FF 00 | 4 pots are at max, 4 are at min |
| 0x07 | Display | send image to display use wifi for faster refresh rate? |  |  |  |
| 0x08 | Actuator |  |  |  |  |
| 0x09 | Telemetry | battery voltage, temperature, mac address |  |  |  |
| 0x10 | Serial |  |  |  |  |
