#pragma once

#include "../sauna360.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace sauna360 {

class SAUNA360Sensor : public SAUNA360Listener, public Component, sensor::Sensor {
 public:
  void dump_config() override;
  void set_temperature_sensor(sensor::Sensor *sensor) { this->temperature_sensor_ = sensor; }
  void on_temperature(uint16_t temperature) override {
    if (this->temperature_sensor_ != nullptr) {
      if (this->temperature_sensor_->get_state() != temperature) {
        this->temperature_sensor_->publish_state(temperature);
      }
    }
   }
  void set_temperature_setting_sensor(sensor::Sensor *sensor) { this->temperature_setting_sensor_ = sensor; }
  void on_temperature_setting(uint16_t temperature_setting) override {
    if (this->temperature_setting_sensor_ != nullptr) {
      if (this->temperature_setting_sensor_->get_state() != temperature_setting) {
        this->temperature_setting_sensor_->publish_state(temperature_setting);
      }
    }
   }
  void set_remaining_time_sensor(sensor::Sensor *sensor) { this->remaining_time_sensor_ = sensor; }
  void on_remaining_time(uint16_t remaining_time) override {
    if (this->remaining_time_sensor_ != nullptr) {
      if (this->remaining_time_sensor_->get_state() != remaining_time) {
        this->remaining_time_sensor_->publish_state(remaining_time);
      }
    }
   }
  void set_humidity_sensor(sensor::Sensor *sensor) { this->humidity_sensor_ = sensor; }
  void on_humidity(uint16_t humidity) override {
    if (this->humidity_sensor_ != nullptr) {
      if (this->humidity_sensor_->get_state() != humidity) {
        this->humidity_sensor_->publish_state(humidity);
      }
    }
  }
  void set_humidity_percentage_sensor(sensor::Sensor *sensor) { this->humidity_percentage_sensor_ = sensor; }
  void on_humidity_percentage(uint16_t humidity_percentage) override {
    if (this->humidity_percentage_sensor_ != nullptr) {
      if (this->humidity_percentage_sensor_->get_state() != humidity_percentage) {
        this->humidity_percentage_sensor_->publish_state(humidity_percentage);
      }
    }
  }
  void set_humidity_percentage_setting_sensor(sensor::Sensor *sensor) { this->humidity_percentage_setting_sensor_ = sensor; }
  void on_humidity_percentage_setting(uint16_t humidity_percentage_setting) override {
    if (this->humidity_percentage_setting_sensor_ != nullptr) {
      if (this->humidity_percentage_setting_sensor_->get_state() != humidity_percentage_setting) {
        this->humidity_percentage_setting_sensor_->publish_state(humidity_percentage_setting);
      }
    }
  }
  void set_bath_time_setting_sensor(sensor::Sensor *sensor) { this->bath_time_setting_sensor_ = sensor; }
  void on_bath_time_setting(uint16_t bath_time_setting) override {
    if (this->bath_time_setting_sensor_ != nullptr) {
      if (this->bath_time_setting_sensor_->get_state() != bath_time_setting) {
        this->bath_time_setting_sensor_->publish_state(bath_time_setting);
      }
    }
  }
  void set_total_uptime_sensor(sensor::Sensor *sensor) { this->total_uptime_sensor_ = sensor; }
  void on_total_uptime(uint16_t total_uptime) override {
    if (this->total_uptime_sensor_ != nullptr) {
      if (this->total_uptime_sensor_->get_state() != total_uptime) {
        this->total_uptime_sensor_->publish_state(total_uptime);
      }
    }
  }
  void set_max_bath_temperature_sensor(sensor::Sensor *sensor) { this->max_bath_temperature_sensor_ = sensor; }
  void on_max_bath_temperature(uint16_t max_bath_temperature) override {
    if (this->max_bath_temperature_sensor_ != nullptr) {
      if (this->max_bath_temperature_sensor_->get_state() != max_bath_temperature) {
        this->max_bath_temperature_sensor_->publish_state(max_bath_temperature);
      }
    }
  }
  void set_overheating_pcb_limit_sensor(sensor::Sensor *sensor) { this->overheating_pcb_limit_sensor_ = sensor; }
  void on_overheating_pcb_limit(uint16_t overheating_pcb_limit) override {
    if (this->overheating_pcb_limit_sensor_ != nullptr) {
      if (this->overheating_pcb_limit_sensor_->get_state() != overheating_pcb_limit) {
        this->overheating_pcb_limit_sensor_->publish_state(overheating_pcb_limit);
      }
    }
  }
  void set_standby_temperature_reduction_sensor(sensor::Sensor *sensor) { this->standby_temperature_reduction_sensor_ = sensor; }
  void on_standby_temperature_reduction(uint16_t standby_temperature_reduction) override {
    if (this->standby_temperature_reduction_sensor_ != nullptr) {
      if (this->standby_temperature_reduction_sensor_->get_state() != standby_temperature_reduction) {
        this->standby_temperature_reduction_sensor_->publish_state(standby_temperature_reduction);
      }
    }
  }
  void set_external_switch_renew_bathtime_sensor(sensor::Sensor *sensor) { this->external_switch_renew_bathtime_sensor_ = sensor; }
  void on_external_switch_renew_bathtime(uint16_t external_switch_renew_bathtime) override {
    if (this->external_switch_renew_bathtime_sensor_ != nullptr) {
      if (this->external_switch_renew_bathtime_sensor_->get_state() != external_switch_renew_bathtime) {
        this->external_switch_renew_bathtime_sensor_->publish_state(external_switch_renew_bathtime);
      }
    }
  }
 protected:
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *temperature_setting_sensor_{nullptr};
  sensor::Sensor *remaining_time_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  sensor::Sensor *humidity_percentage_sensor_{nullptr};
  sensor::Sensor *humidity_percentage_setting_sensor_{nullptr};
  sensor::Sensor *bath_time_setting_sensor_{nullptr};
  sensor::Sensor *total_uptime_sensor_{nullptr};
  sensor::Sensor *max_bath_temperature_sensor_{nullptr};
  sensor::Sensor *overheating_pcb_limit_sensor_{nullptr};
  sensor::Sensor *standby_temperature_reduction_sensor_{nullptr};
  sensor::Sensor *external_switch_renew_bathtime_sensor_{nullptr};
};

}  // namespace sauna360
}  // namespace esphome