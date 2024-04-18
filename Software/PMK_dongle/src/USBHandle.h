#ifndef USBHANDLE_H
#define USBHANDLE_H

#include "Arduino.h"
#include "variables.h"
//#include "USB.h"

// HID report descriptor using TinyUSB's template
uint8_t const desc_keyboard_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

uint8_t const desc_mouse_report[] = {
  TUD_HID_REPORT_DESC_MOUSE()
};

uint8_t const desc_gamepad_report[] = {
  TUD_HID_REPORT_DESC_GAMEPAD()
};

// Output report callback for LED indicator such as Caplocks
void hid_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) report_id;
  (void) bufsize;

  // LED indicator is output report with only 1 byte length
  if ( report_type != HID_REPORT_TYPE_OUTPUT ) return;

  // The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
  // Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
  uint8_t ledIndicator = buffer[0];
  //Serial.printf("bufsize: %i, sizeof buffer: %i\r\n", bufsize, sizeof(buffer));
  // turn on LED if capslock is set
  digitalWrite(LED_DATA_PIN, ledIndicator);
}


//static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
//  if(event_base == ARDUINO_USB_EVENTS){
//    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
//    switch (event_id){
//      case ARDUINO_USB_STARTED_EVENT:
//        //HWSerial.println("USB PLUGGED");
//        break;
//      case ARDUINO_USB_STOPPED_EVENT:
//        digitalWrite(LED_DATA_PIN, 0);
//        //HWSerial.println("USB UNPLUGGED");
//        break;
//      case ARDUINO_USB_SUSPEND_EVENT:
//        //HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
//        break;
//      case ARDUINO_USB_RESUME_EVENT:
//        //HWSerial.println("USB RESUMED");
//        break;
//      
//      default:
//        break;
//    }
//  } else if(event_base == ARDUINO_USB_CDC_EVENTS){
//    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;
//    switch (event_id){
//      case ARDUINO_USB_CDC_CONNECTED_EVENT:
//        //HWSerial.println("CDC CONNECTED");
//        break;
//      case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
//        //HWSerial.println("CDC DISCONNECTED");
//        break;
//      case ARDUINO_USB_CDC_LINE_STATE_EVENT:
//        //HWSerial.printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
//        break;
//      case ARDUINO_USB_CDC_LINE_CODING_EVENT:
//        //HWSerial.printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
//        break;
//      case ARDUINO_USB_CDC_RX_EVENT:
//        //HWSerial.printf("CDC RX [%u]:", data->rx.len);
//        {
//            uint8_t buf[data->rx.len];
//            size_t len = Serial.read(buf, data->rx.len);
//            //HWSerial.write(buf, len);
//        }
//        //HWSerial.println();
//        break;
//       case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
//        //HWSerial.printf("CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
//        break;
//     
//      default:
//        break;
//    }
//  }/* else if(event_base == ARDUINO_FIRMWARE_MSC_EVENTS){
//    arduino_firmware_msc_event_data_t * data = (arduino_firmware_msc_event_data_t*)event_data;
//    switch (event_id){
//      case ARDUINO_FIRMWARE_MSC_START_EVENT:
//        Serial.println("MSC Update Start");
//        break;
//      case ARDUINO_FIRMWARE_MSC_WRITE_EVENT:
//        //HWSerial.printf("MSC Update Write %u bytes at offset %u\n", data->write.size, data->write.offset);
//        Serial.print(".");
//        break;
//      case ARDUINO_FIRMWARE_MSC_END_EVENT:
//        Serial.printf("\nMSC Update End: %u bytes\n", data->end.size);
//        break;
//      case ARDUINO_FIRMWARE_MSC_ERROR_EVENT:
//        Serial.printf("MSC Update ERROR! Progress: %u bytes\n", data->error.size);
//        break;
//      case ARDUINO_FIRMWARE_MSC_POWER_EVENT:
//        Serial.printf("MSC Update Power: power: %u, start: %u, eject: %u", data->power.power_condition, data->power.start, data->power.load_eject);
//        break;
//      
//      default:
//        break;
//    }
//  }
//  */
//}

#endif