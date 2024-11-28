#include "sauna360_select.h"

namespace esphome {
namespace sauna360 {


static const char *const TAG = "SAUNA360.select";

void Sauna360RelayAux0Select::control(const std::string &value) {
  this->publish_state(value);
  this->parent_->set_aux0_relay_mode(value);
}

void Sauna360RelayAux1Select::control(const std::string &value) {
  this->publish_state(value);
  this->parent_->set_aux1_relay_mode(value);
}

void Sauna360RelayAux2Select::control(const std::string &value) {
  this->publish_state(value);
  this->parent_->set_aux2_relay_mode(value);
}

void Sauna360ExternalSwitchSelect::control(const std::string &value) {
  this->publish_state(value);
  this->parent_->set_external_switching_mode(value);
}

}  // namespace sauna360
}  // namespace esphome