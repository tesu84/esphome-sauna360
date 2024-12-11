#pragma once

#include "esphome/components/number/number.h"
#include "../sauna360.h"

namespace esphome {
namespace sauna360 {

class SAUNA360BathTimeNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360BathTimeNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360BathTemperatureNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360BathTemperatureNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360StandbyTemperatureReductionNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360StandbyTemperatureReductionNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360HumidityStepNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360HumidityStepNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360MaxBathTemperatureNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360MaxBathTemperatureNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360OverheatingPCBLimitNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360OverheatingPCBLimitNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360ExternalSwitchRenewBathTimeNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360ExternalSwitchRenewBathTimeNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360Aux0FragrancePumpNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux0FragrancePumpNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360Aux0FragranceStopNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux0FragranceStopNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360Aux1FragrancePumpNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux1FragrancePumpNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360Aux1FragranceStopNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux1FragranceStopNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360Aux2FragrancePumpNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux2FragrancePumpNumber() = default;

 protected:
  void control(float value) override;
};

class SAUNA360Aux2FragranceStopNumber : public number::Number, public Parented<SAUNA360Component> {
 public:
  SAUNA360Aux2FragranceStopNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace sauna360
}  // namespace esphome