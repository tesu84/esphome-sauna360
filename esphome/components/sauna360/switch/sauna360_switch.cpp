#include "sauna360_switch.h"

namespace esphome {
namespace sauna360 {

void SAUNA360LightRelaySwitch::write_state(bool state) {
    this->publish_state(state);
    this->parent_->set_light_relay(state);
  }

void SAUNA360Aux0RelaySwitch::write_state(bool state) {
    this->publish_state(state);
    this->parent_->set_aux0_relay(state);
  }

void SAUNA360Aux1RelaySwitch::write_state(bool state) {
    this->publish_state(state);
    this->parent_->set_aux1_relay(state);
  }

void SAUNA360Aux2RelaySwitch::write_state(bool state) {
    this->publish_state(state);
    this->parent_->set_aux2_relay(state);
  }

void SAUNA360StandbyEnableSwitch::write_state(bool state) {
    this->publish_state(state);
    this->parent_->set_standby_enable(state);
  }

void SAUNA360ActivateTimeLimitSwitch::write_state(bool state) {
    this->publish_state(state);
    this->parent_->set_activate_time_limit(state);
  }

}  // namespace sauna360
}  // namespace esphome