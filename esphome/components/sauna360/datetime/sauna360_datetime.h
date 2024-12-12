#pragma once

#include "../sauna360.h"

#include "esphome/core/defines.h"

#include "esphome/components/datetime/datetime_entity.h"
#include "esphome/components/datetime/time_entity.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "esphome/core/time.h"

namespace esphome {
namespace sauna360 {

class SAUNA360DateTime : public datetime::DateTimeEntity, public Component, public SAUNA360Listener, public Parented<SAUNA360Component> {
 public:
  void on_datetime(ESPTime &f) override { 
    this->f_ = f; 
    if (!this->f_.has_value())
        return;

    auto val = this->f_;
    if (!val.has_value())
      return;

    this->year_ = val->year;
    this->month_ = val->month;
    this->day_ = val->day_of_month;
    this->hour_ = val->hour;
    this->minute_ = val->minute;
    this->publish_state();
  }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  Trigger<ESPTime> *get_set_trigger() const { return this->set_trigger_; }
  void set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }

  void set_initial_value(ESPTime initial_value) { this->initial_value_ = initial_value; }
  void set_restore_value(bool restore_value) { this->restore_value_ = restore_value; }

 protected:
  void control(const datetime::DateTimeCall &call) override;

  bool optimistic_{true};
  ESPTime initial_value_{};
  bool restore_value_{false};
  Trigger<ESPTime> *set_trigger_ = new Trigger<ESPTime>();
  optional<ESPTime> f_;

  ESPPreferenceObject pref_;
};

class SAUNA360NotAllowedStartFromTime : public datetime::TimeEntity, public Component, public SAUNA360Listener, public Parented<SAUNA360Component> {
 public:
  void on_not_allowed_start_from_time(ESPTime &f) override { 
    this->f_ = f; 
    if (!this->f_.has_value())
    return;

    auto val = this->f_;
    if (!val.has_value())
    return;

  this->hour_ = val->hour;
  this->minute_ = val->minute;
  this->publish_state();
  }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  Trigger<ESPTime> *get_set_trigger() const { return this->set_trigger_; }
  void set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }

  void set_initial_value(ESPTime initial_value) { this->initial_value_ = initial_value; }
  void set_restore_value(bool restore_value) { this->restore_value_ = restore_value; }

 protected:
  void control(const datetime::TimeCall &call) override;

  bool optimistic_{true};
  ESPTime initial_value_{};
  bool restore_value_{false};
  Trigger<ESPTime> *set_trigger_ = new Trigger<ESPTime>();
  optional<ESPTime> f_;

  ESPPreferenceObject pref_;
};

class SAUNA360NotAllowedStartUntilTime : public datetime::TimeEntity, public Component, public SAUNA360Listener, public Parented<SAUNA360Component> {
 public:
  void on_not_allowed_start_until_time(ESPTime &f) override { 
    this->f_ = f; 
    if (!this->f_.has_value())
    return;

    auto val = this->f_;
    if (!val.has_value())
    return;

  this->hour_ = val->hour;
  this->minute_ = val->minute;
  this->publish_state();
  }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  Trigger<ESPTime> *get_set_trigger() const { return this->set_trigger_; }
  void set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }

  void set_initial_value(ESPTime initial_value) { this->initial_value_ = initial_value; }
  void set_restore_value(bool restore_value) { this->restore_value_ = restore_value; }

 protected:
  void control(const datetime::TimeCall &call) override;

  bool optimistic_{true};
  ESPTime initial_value_{};
  bool restore_value_{false};
  Trigger<ESPTime> *set_trigger_ = new Trigger<ESPTime>();
  optional<ESPTime> f_;

  ESPPreferenceObject pref_;
};

}  // namespace sauna360
}  // namespace esphome