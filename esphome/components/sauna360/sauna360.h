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
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef USE_DATETIME
#include "esphome/components/datetime/datetime_entity.h"
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
   virtual void on_bath_time_setting (uint16_t bath_time_setting){};
   virtual void on_total_uptime (uint16_t bath_time_setting){};
   virtual void on_max_bath_temperature (uint16_t max_bath_temperature){};
   virtual void on_overheating_pcb_limit (uint16_t overheating_pcb_limit){};
   virtual void on_standby_temperature_reduction (uint16_t standby_temp_reduction){};
   virtual void on_external_switch_renew_bathtime (uint16_t external_switch_renew_bathtime){};
   virtual void on_heater_status(bool heater_status){};
   virtual void on_heater_state(std::string &fw){};
   virtual void on_water_level(std::string &fw){};
   virtual void on_light_status(bool light_status){};
   virtual void on_ready_status(bool ready_status){};
   virtual void on_relay_x3_x4_status(bool relay_x3_x4_status){};
   virtual void on_relay_x5_x6_status(bool relay_x5_x6_status){};
   virtual void on_relay_x7_x8_status(bool relay_x7_x8_status){};
   virtual void on_relay_x9_x10_status(bool relay_x9_x10_status){};
   virtual void on_relay_x11_x12_status(bool relay_x11_x12_status){};
   virtual void on_relay_x13_x14_status(bool relay_x13_x14_status){};
   virtual void on_relay_x15_x16_status(bool relay_x15_x16_status){};
   virtual void on_relay_x17_x18_status(bool relay_x17_x18_status){};
   virtual void on_datetime(ESPTime &f){};
   virtual void on_not_allowed_start_from_time(ESPTime &f){};
   virtual void on_not_allowed_start_until_time(ESPTime &f){};
};

class SAUNA360Component : public uart::UARTDevice, public Component {

  #ifdef USE_NUMBER
    SUB_NUMBER(bath_time)
    SUB_NUMBER(bath_temperature)
    SUB_NUMBER(standby_temperature_reduction)
    SUB_NUMBER(humidity_step)
    SUB_NUMBER(max_bath_temperature)
    SUB_NUMBER(overheating_pcb_limit)
    SUB_NUMBER(external_switch_renew_bathtime)
  #endif
  #ifdef USE_SWITCH
    SUB_SWITCH(light_relay)
    SUB_SWITCH(aux0_relay)
    SUB_SWITCH(aux1_relay)
    SUB_SWITCH(aux2_relay)
    SUB_SWITCH(standby_enable)
    SUB_SWITCH(activate_time_limit)
  #endif
  #ifdef USE_SELECT
   SUB_SELECT(aux0_relay_mode)
   SUB_SELECT(aux1_relay_mode)
   SUB_SELECT(aux2_relay_mode)
   SUB_SELECT(external_switching_mode)
   SUB_SELECT(bath_type_priority)
  #endif
  #ifdef USE_BUTTON
   SUB_BUTTON(heater_on)
   SUB_BUTTON(heater_off)
   SUB_BUTTON(heater_standby)
   SUB_BUTTON(heater_power_toggle)
  #endif

  public:
    void setup() override;
    void loop() override;
    void dump_config() override;
    void register_listener(SAUNA360Listener *listener) { this->listeners_.push_back(listener); }
    void set_heater_on();
    void set_heater_off();
    void set_heater_standby();
    void set_heater_power_toggle();
    void set_bath_time_number(float value);
    void set_bath_time_default_value(float bath_time_default) { bath_time_default_ = bath_time_default; }
    void set_bath_temperature_number(float value);
    void set_bath_temperature_default_value(float bath_temperature_default) { bath_temperature_default_ = bath_temperature_default; }
    void set_standby_temperature_reduction_number(float value);
    void set_standby_temperature_reduction_default_value(float standby_temperature_reduction_default) { standby_temperature_reduction_default_ = standby_temperature_reduction_default; }
    void set_humidity_step_number(float value);
    void set_humidity_step_default_value(float humidity_step_default) { humidity_step_default_ = humidity_step_default; }
    void set_max_bath_temperature_number(float value);
    void set_max_bath_temperature_default_value(float max_bath_temperature_default) { max_bath_temperature_default_ = max_bath_temperature_default; }
    void set_overheating_pcb_limit_number(float value);
    void set_overheating_pcb_limit_default_value(float overheating_pcb_limit_default) { overheating_pcb_limit_default_ = overheating_pcb_limit_default; }
    void set_external_switch_renew_bathtime_number(float value);
    void set_external_switch_renew_bathtime_default_value(float external_switch_renew_bathtime_default) { external_switch_renew_bathtime_default_ = external_switch_renew_bathtime_default; }
    void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
    void set_light_relay(bool enable);
    void set_aux0_relay(bool enable);
    void set_aux0_relay_mode(const std::string &state);
    void set_aux1_relay(bool enable);
    void set_aux1_relay_mode(const std::string &state);
    void set_aux2_relay(bool enable);
    void set_aux2_relay_mode(const std::string &state);
    void set_external_switching_mode(const std::string &state);
    void set_bath_type_priority(const std::string &state);
    void set_standby_enable(bool enable);
    void set_activate_time_limit(bool enable);
    void set_datetime(ESPTime &time);
    void set_not_allowed_start_from_time(ESPTime &time);
    void set_not_allowed_start_until_time(ESPTime &time);

  protected:
    GPIOPin *flow_control_pin_{nullptr};
    void handle_char_(uint8_t c);
    void handle_frame_(std::vector<uint8_t> frame);
    bool frame_flag_;
    void handle_packet_(std::vector<uint8_t> packet);
    void send_data_();
    void create_send_data_(uint8_t type, uint16_t code, uint32_t data);
    std::vector<uint8_t> rx_message_;
    std::queue<std::vector<uint8_t>> tx_queue_;
    uint32_t last_tx_;
    uint32_t temperature_received_hex_;
    uint32_t setpoint_temperature_received_hex_;
    uint32_t standby_temperature_reduction_received_hex_;
    uint32_t bath_time_received_hex_;
    uint32_t max_bath_temperature_received_hex_;
    uint32_t external_switch_renew_bathtime_received_hex_;
    uint32_t overheating_pcb_limit_received_hex_;
    uint32_t bath_type_priority_received_hex_;
    ESPTime time_limit_from_;
    ESPTime time_limit_until_;
    bool activate_time_limit_;
    std::vector<SAUNA360Listener *> listeners_{};
    float bath_time_default_{NAN};
    float bath_temperature_default_{NAN};
    float standby_temperature_reduction_default_{NAN};
    float humidity_step_default_{NAN};
    float max_bath_temperature_default_{NAN};
    float overheating_pcb_limit_default_{NAN};
    float external_switch_renew_bathtime_default_{NAN};
};


}  // namespace sauna360
}  // namespace esphome