#include "dooya_a3_cover.h"

namespace esphome {
  namespace dooya_a3_cover {
    void DooyaA3Cover::setup() {

    }

    void DooyaA3Cover::update() {

    }

    void DooyaA3Cover::loop() {

    }

    cover::CoverTraits DooyaA3Cover::get_traits() {
      auto traits = cover::CoverTraits();
      traits.set_is_assumed_state(false);
      traits.set_supports_position(true);
      traits.set_supports_tilt(false);
      traits.set_supports_stop(true);
      return traits;
    }

    void DooyaA3Cover::control(const cover::CoverCall &call) {
      if (call.get_position().has_value()) {
        float pos = *call.get_position();
        int scaled_pos = static_cast<int>(pos * 100); // Scale and convert to int
        if (scaled_pos > 100) {
          scaled_pos = 100; // Ensure the value does not exceed 100
        }

        if (scaled_pos == 100) { // open
          uint8_t open_cmd[7] = {0x55, id_low_, id_high_, 0x03, 0x01, 0x00, 0x00};
          uint16_t crc = crc16(open_cmd, sizeof(open_cmd) - 2);
          open_cmd[5] = static_cast<uint8_t>(crc & 0x00FF);
          open_cmd[6] = static_cast<uint8_t>((crc & 0xFF00) >> 8);
          this->uart_->write_array(open_cmd, sizeof(open_cmd));
          ESP_LOGD(TAG, "send open cmd: %s", format_uart_data(open_cmd, sizeof(open_cmd)));
        } else if (scaled_pos == 0) { // close
          uint8_t close_cmd[7] = {0x55, 0x12, 0x34, 0x03, 0x02, 0x00, 0x00};
          uint16_t crc = crc16(close_cmd, sizeof(close_cmd) - 2);
          close_cmd[5] = static_cast<uint8_t>(crc & 0x00FF);
          close_cmd[6] = static_cast<uint8_t>((crc & 0xFF00) >> 8);
          this->uart_->write_array(close_cmd, sizeof(close_cmd));
          ESP_LOGD(TAG, "send close cmd: %s", format_uart_data(close_cmd, sizeof(close_cmd)));
        } else {
          uint8_t hex_pos = static_cast<uint8_t>(scaled_pos); // Convert to uint8_t for hexadecimal representation

          uint8_t pos_cmd[8] = {0x55, id_low_, id_high_, 0x03, 0x04, 0x00, 0x00, 0x00};
          pos_cmd[5] = hex_pos;
          uint16_t crc = crc16(pos_cmd, sizeof(pos_cmd) - 2);
          pos_cmd[6] = static_cast<uint8_t>(crc & 0x00FF);
          pos_cmd[7] = static_cast<uint8_t>((crc & 0xFF00) >> 8);
          this->uart_->write_array(pos_cmd, sizeof(pos_cmd));
          ESP_LOGD(TAG, "send pos cmd: %s", format_uart_data(pos_cmd, sizeof(pos_cmd)));
        }
        this->position = pos;
      }

      if (call.get_stop()) {
        // stop
        uint8_t stop_cmd[7] = {0x55, id_low_, id_high_, 0x03, 0x03, 0x00, 0x00};
        uint16_t crc = crc16(stop_cmd, sizeof(stop_cmd) - 2);
        stop_cmd[5] = static_cast<uint8_t>(crc & 0x00FF);
        stop_cmd[6] = static_cast<uint8_t>((crc & 0xFF00) >> 8);
        this->uart_->write_array(stop_cmd, sizeof(stop_cmd));
        ESP_LOGD(TAG, "send stop cmd: %s", format_uart_data(stop_cmd, sizeof(stop_cmd)));
      }

      this->publish_state();
    }

    uint16_t crc16(uint8_t *data, uint8_t length) {
      uint16_t crc = 0xFFFF;

      for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 8; j > 0; j--) {
          if (crc & 0x0001) {
            crc = (crc >> 1) ^ 0xA001;
          } else {
            crc >>= 1;
          }
        }
      }

      return crc;
    }

    char* format_uart_data(uint8_t *uart_data, int length) {
      static char str[256] = {0};  // Output buffer
      char tmp[10];  // Temporary buffer

      str[0] = '\0';  // Clear the buffer
      for (int i = 0; i < length; i++) {
        sprintf(tmp, "%x:", uart_data[i]);
        strcat(str, tmp);  // Append to str
      }

      str[strlen(str)-1] = '\0';  // Replace the last colon with a null terminator

      return str;
    }

  }
}
