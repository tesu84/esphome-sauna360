#include "sauna360_button.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

static const char *const TAG = "SAUNA360.button";

namespace esphome {
namespace sauna360 {

void SAUNA360HeaterOnButton::press_action() { this->parent_->set_heater_on(); }
void SAUNA360HeaterOffButton::press_action() { this->parent_->set_heater_off(); }
void SAUNA360HeaterStandbyButton::press_action() { this->parent_->set_heater_standby(); }
void SAUNA360HeaterPowerToggleButton::press_action() { this->parent_->set_heater_power_toggle(); }

}  // namespace sauna360
}  // namespace esphome