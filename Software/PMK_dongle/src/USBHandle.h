#include "Arduino.h"
//#include "USB.h"

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        //HWSerial.println("USB PLUGGED");
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        digitalWrite(LED_DATA_PIN, 0);
        //HWSerial.println("USB UNPLUGGED");
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        //HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        //HWSerial.println("USB RESUMED");
        break;
      
      default:
        break;
    }
  } else if(event_base == ARDUINO_USB_CDC_EVENTS){
    arduino_usb_cdc_event_data_t * data = (arduino_usb_cdc_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_CDC_CONNECTED_EVENT:
        //HWSerial.println("CDC CONNECTED");
        break;
      case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
        //HWSerial.println("CDC DISCONNECTED");
        break;
      case ARDUINO_USB_CDC_LINE_STATE_EVENT:
        //HWSerial.printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
        break;
      case ARDUINO_USB_CDC_LINE_CODING_EVENT:
        //HWSerial.printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
        break;
      case ARDUINO_USB_CDC_RX_EVENT:
        //HWSerial.printf("CDC RX [%u]:", data->rx.len);
        {
            uint8_t buf[data->rx.len];
            size_t len = Serial.read(buf, data->rx.len);
            //HWSerial.write(buf, len);
        }
        //HWSerial.println();
        break;
       case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
        //HWSerial.printf("CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
        break;
     
      default:
        break;
    }
  }/* else if(event_base == ARDUINO_FIRMWARE_MSC_EVENTS){
    arduino_firmware_msc_event_data_t * data = (arduino_firmware_msc_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_FIRMWARE_MSC_START_EVENT:
        Serial.println("MSC Update Start");
        break;
      case ARDUINO_FIRMWARE_MSC_WRITE_EVENT:
        //HWSerial.printf("MSC Update Write %u bytes at offset %u\n", data->write.size, data->write.offset);
        Serial.print(".");
        break;
      case ARDUINO_FIRMWARE_MSC_END_EVENT:
        Serial.printf("\nMSC Update End: %u bytes\n", data->end.size);
        break;
      case ARDUINO_FIRMWARE_MSC_ERROR_EVENT:
        Serial.printf("MSC Update ERROR! Progress: %u bytes\n", data->error.size);
        break;
      case ARDUINO_FIRMWARE_MSC_POWER_EVENT:
        Serial.printf("MSC Update Power: power: %u, start: %u, eject: %u", data->power.power_condition, data->power.start, data->power.load_eject);
        break;
      
      default:
        break;
    }
  }
  */
}
