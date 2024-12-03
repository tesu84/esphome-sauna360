#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "../sauna360.h"

namespace esphome {
namespace sauna360 {

class SAUNA360BinarySensor : public SAUNA360Listener, public Component, binary_sensor::BinarySensor {
 public:
  void dump_config() override;

  void set_heater_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->heater_bsensor_ = bsensor; };
  void on_heater_status(bool heater_status) override {
    if (this->heater_bsensor_ != nullptr) {
      if (this->heater_bsensor_->state != heater_status) {
        this->heater_bsensor_->publish_state(heater_status);
      }
    }
   }

  void set_light_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->light_bsensor_ = bsensor; };
  void on_light_status(bool light_status) override {
    if (this->light_bsensor_ != nullptr) {
      if (this->light_bsensor_->state != light_status) {
        this->light_bsensor_->publish_state(light_status);
      }
    }
   }

  void set_ready_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->ready_bsensor_ = bsensor; };
  void on_ready_status(bool ready_status) override {
    if (this->ready_bsensor_ != nullptr) {
      if (this->ready_bsensor_->state != ready_status) {
        this->ready_bsensor_->publish_state(ready_status);
      }
    }
   }

  void set_relay_x3_x4_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->relay_x3_x4_bsensor_ = bsensor; };
  void on_relay_x3_x4_status(bool relay_x3_x4_status) override {
    if (this->relay_x3_x4_bsensor_ != nullptr) {
      if (this->relay_x3_x4_bsensor_->state != relay_x3_x4_status) {
        this->relay_x3_x4_bsensor_->publish_state(relay_x3_x4_status);
      }
    }
   }

  void set_relay_x5_x6_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->relay_x5_x6_bsensor_ = bsensor; };
  void on_relay_x5_x6_status(bool relay_x5_x6_status) override {
    if (this->relay_x5_x6_bsensor_ != nullptr) {
      if (this->relay_x5_x6_bsensor_->state != relay_x5_x6_status) {
        this->relay_x5_x6_bsensor_->publish_state(relay_x5_x6_status);
      }
    }
   }
  
    void set_relay_x7_x8_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->relay_x7_x8_bsensor_ = bsensor; };
  void on_relay_x7_x8_status(bool relay_x7_x8_status) override {
    if (this->relay_x7_x8_bsensor_ != nullptr) {
      if (this->relay_x7_x8_bsensor_->state != relay_x7_x8_status) {
        this->relay_x7_x8_bsensor_->publish_state(relay_x7_x8_status);
      }
    }
   }
 
  void set_relay_x9_x10_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->relay_x9_x10_bsensor_ = bsensor; };
  void on_relay_x9_x10_status(bool relay_x9_x10_status) override {
    if (this->relay_x9_x10_bsensor_ != nullptr) {
      if (this->relay_x9_x10_bsensor_->state != relay_x9_x10_status) {
        this->relay_x9_x10_bsensor_->publish_state(relay_x9_x10_status);
      }
    }
   }

  void set_relay_x11_x12_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->relay_x11_x12_bsensor_ = bsensor; };
  void on_relay_x11_x12_status(bool relay_x11_x12_status) override {
    if (this->relay_x11_x12_bsensor_ != nullptr) {
      if (this->relay_x11_x12_bsensor_->state != relay_x11_x12_status) {
        this->relay_x11_x12_bsensor_->publish_state(relay_x11_x12_status);
      }
    }
   }

  void set_relay_x13_x14_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->relay_x13_x14_bsensor_ = bsensor; };
  void on_relay_x13_x14_status(bool relay_x13_x14_status) override {
    if (this->relay_x13_x14_bsensor_ != nullptr) {
      if (this->relay_x13_x14_bsensor_->state != relay_x13_x14_status) {
        this->relay_x13_x14_bsensor_->publish_state(relay_x13_x14_status);
      }
    }
   }

  void set_relay_x15_x16_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->relay_x15_x16_bsensor_ = bsensor; };
  void on_relay_x15_x16_status(bool relay_x15_x16_status) override {
    if (this->relay_x15_x16_bsensor_ != nullptr) {
      if (this->relay_x15_x16_bsensor_->state != relay_x15_x16_status) {
        this->relay_x15_x16_bsensor_->publish_state(relay_x15_x16_status);
      }
    }
   }

  void set_relay_x17_x18_binary_sensor(binary_sensor::BinarySensor *bsensor) { this->relay_x17_x18_bsensor_ = bsensor; };
  void on_relay_x17_x18_status(bool relay_x17_x18_status) override {
    if (this->relay_x17_x18_bsensor_ != nullptr) {
      if (this->relay_x17_x18_bsensor_->state != relay_x17_x18_status) {
        this->relay_x17_x18_bsensor_->publish_state(relay_x17_x18_status);
      }
    }
   }

 protected:
  binary_sensor::BinarySensor *heater_bsensor_{nullptr};
  binary_sensor::BinarySensor *light_bsensor_{nullptr};
  binary_sensor::BinarySensor *ready_bsensor_{nullptr};
  binary_sensor::BinarySensor *relay_x3_x4_bsensor_{nullptr};
  binary_sensor::BinarySensor *relay_x5_x6_bsensor_{nullptr};
  binary_sensor::BinarySensor *relay_x7_x8_bsensor_{nullptr};
  binary_sensor::BinarySensor *relay_x9_x10_bsensor_{nullptr};
  binary_sensor::BinarySensor *relay_x11_x12_bsensor_{nullptr};
  binary_sensor::BinarySensor *relay_x13_x14_bsensor_{nullptr};
  binary_sensor::BinarySensor *relay_x15_x16_bsensor_{nullptr};
  binary_sensor::BinarySensor *relay_x17_x18_bsensor_{nullptr};
};

}  // namespace sauna360
}  // namespace esphome