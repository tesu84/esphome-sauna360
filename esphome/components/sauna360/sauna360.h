#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"
#include "esphome/core/gpio.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif

#include <queue>
#include <map>
#include <functional>

namespace esphome {
namespace sauna360 {

class SAUNA360Listener {
 public:
   virtual void on_temperature(uint16_t temperature){};
   virtual void on_temperature_setting(uint16_t temperature_setting){};
   virtual void on_remaining_time(uint16_t remaining_time){};
   virtual void on_humidity(uint16_t humidity){};
   virtual void on_humidity_percentage(uint16_t humidity_percentage){};
   virtual void on_heater_status(bool heater_status){};
   virtual void on_light_status(bool light_status){};
   virtual void on_ready_status(bool ready_status){};
};

class SAUNA360Component : public uart::UARTDevice, public Component {

   #ifdef USE_NUMBER
    SUB_NUMBER(bath_time)
    SUB_NUMBER(bath_temperature)
   #endif

  public:
    void setup() override;
    void loop() override;
    void dump_config() override;
    void register_listener(SAUNA360Listener *listener) { this->listeners_.push_back(listener); }
   #ifdef USE_BUTTON
    void set_heater_on_button(button::Button *button) { this->set_elite_heater_on_button_ = button; };
    button::Button *set_elite_heater_on_button_{nullptr};
    void set_heater_off_button(button::Button *button) { this->set_elite_heater_off_button_ = button; };
    button::Button *set_elite_heater_off_button_{nullptr};
    void set_heater_standby_button(button::Button *button) { this->set_elite_heater_standby_button_ = button; };
    button::Button *set_elite_heater_standby_button_{nullptr};
    void set_heater_power_toggle_button(button::Button *button) { this->set_pure_power_toggle_button_ = button; };
    button::Button *set_pure_power_toggle_button_{nullptr};
   #endif
    void apply_heater_on_action();
    void apply_heater_off_action();
    void apply_heater_standby_action();
    void apply_heater_power_toggle_action();
    void set_bath_time_number(float value);
    void set_bath_time_default_value(float bath_time_default) { bath_time_default_ = bath_time_default; }
    void set_bath_temperature_number(float value);
    void set_bath_temperature_default_value(float bath_temperature_default) { bath_temperature_default_ = bath_temperature_default; }
    void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }

  protected:
    GPIOPin *flow_control_pin_{nullptr};
    bool flow_control_pin_is_set_;
    void handle_char_(uint8_t c);
    void handle_frame_(std::vector<uint8_t> frame);
    void handle_packet_(std::vector<uint8_t> packet);
    void send_data_();
    void create_send_data_(uint8_t type, uint16_t code, uint32_t data);
    std::vector<uint8_t> rx_message_;
    std::vector<uint8_t> byte_swapped_packet_;
    std::queue<std::vector<uint8_t>> tx_queue_;
    uint32_t last_rx_;
    uint32_t last_tx_;
    uint32_t last_frame_;
    std::vector<SAUNA360Listener *> listeners_{};

    float bath_time_default_{NAN};
    float bath_temperature_default_{NAN};

};


}  // namespace sauna360
}  // namespace esphome