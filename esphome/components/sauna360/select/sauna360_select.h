#pragma once

#include "esphome/components/select/select.h"
#include "../sauna360.h"

namespace esphome {
namespace sauna360 {

class Sauna360RelayAux0Select : public Component, public select::Select, public Parented<SAUNA360Component> {
 public:
  Sauna360RelayAux0Select() = default;

 protected:
  void control(const std::string &value) override;
};

class Sauna360RelayAux1Select : public Component, public select::Select, public Parented<SAUNA360Component> {
 public:
  Sauna360RelayAux1Select() = default;

 protected:
  void control(const std::string &value) override;
};

class Sauna360RelayAux2Select : public Component, public select::Select, public Parented<SAUNA360Component> {
 public:
  Sauna360RelayAux2Select() = default;

 protected:
  void control(const std::string &value) override;
};

class Sauna360ExternalSwitchSelect : public Component, public select::Select, public Parented<SAUNA360Component> {
 public:
  Sauna360ExternalSwitchSelect() = default;

 protected:
  void control(const std::string &value) override;
};

class Sauna360BathTypePrioritySelect : public Component, public select::Select, public Parented<SAUNA360Component> {
 public:
  Sauna360BathTypePrioritySelect() = default;

 protected:
  void control(const std::string &value) override;
};

}  // namespace sauna360
}  // namespace esphome