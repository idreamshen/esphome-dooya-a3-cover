#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/cover/cover.h"

#define TAG "dooya_a3_cover"

namespace esphome {
  namespace dooya_a3_cover {
    class DooyaA3Cover : public PollingComponent, public cover::Cover, public uart::UARTDevice {
    public:
      DooyaA3Cover(uart::UARTComponent *uartComponent) : PollingComponent(60000), UARTDevice(uartComponent) {
        this->uart_ = uartComponent;
      };
      void setup() override;
      void update() override;
      void loop() override;
      cover::CoverTraits get_traits();
      void control(const cover::CoverCall &call) override;
      void set_id_low(uint8_t id_low) {
        this->id_low_ = id_low;
      };
      void set_id_high(uint8_t id_high) {
        this->id_high_ = id_high;
      };

    protected:
      uart::UARTComponent* uart_;
      uint8_t id_low_;
      uint8_t id_high_;
    };

    uint16_t crc16(uint8_t *data, uint8_t length);
    char* format_uart_data(uint8_t *uart_data, int length);
  }
}
