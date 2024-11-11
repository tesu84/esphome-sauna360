#include "sauna360_button.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

static const char *const TAG = "SAUNA360.button";

namespace esphome {
namespace sauna360 {

void SAUNA360HeaterOnButton::press_action() { this->parent_->apply_heater_on_action(); }
void SAUNA360HeaterOffButton::press_action() { this->parent_->apply_heater_off_action(); }
void SAUNA360HeaterStandbyButton::press_action() { this->parent_->apply_heater_standby_action(); }
void SAUNA360HeaterPowerToggleButton::press_action() { this->parent_->apply_heater_power_toggle_action(); }

}  // namespace sauna360
}  // namespace esphome