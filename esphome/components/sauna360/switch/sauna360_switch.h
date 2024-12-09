#pragma once

#include "esphome/components/switch/switch.h"
#include "../sauna360.h"

namespace esphome {
namespace sauna360 {

class SAUNA360LightRelaySwitch : public switch_::Switch, public Parented<SAUNA360Component> {
 public:
  SAUNA360LightRelaySwitch() = default;

 protected:
  void write_state(bool state) override;
};
class SAUNA360Aux0RelaySwitch : public switch_::Switch, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux0RelaySwitch() = default;

 protected:
  void write_state(bool state) override;
};
class SAUNA360Aux1RelaySwitch : public switch_::Switch, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux1RelaySwitch() = default;

 protected:
  void write_state(bool state) override;
};
class SAUNA360Aux2RelaySwitch : public switch_::Switch, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux2RelaySwitch() = default;

 protected:
  void write_state(bool state) override;
};

class SAUNA360StandbyEnableSwitch : public switch_::Switch, public Parented<SAUNA360Component> {
 public:
  SAUNA360StandbyEnableSwitch() = default;

 protected:
  void write_state(bool state) override;
};

class SAUNA360ActivateTimeLimitSwitch : public switch_::Switch, public Parented<SAUNA360Component> {
 public:
  SAUNA360ActivateTimeLimitSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace sauna360
}  // namespace esphome