#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/components/uart/uart_component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
#include "esphome/core/preferences.h"
#include "esphome/core/automation.h"
#include "esphome/core/time.h"
#include "sauna360.h"


namespace esphome {
namespace sauna360 {

static const char *TAG = "sauna360";

void SAUNA360Component::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
  if (!std::isnan(this->max_bath_temperature_default_)) {
    this->set_max_bath_temperature_number(max_bath_temperature_default_);
  }
  if (!std::isnan(this->bath_time_default_)) {
    this->set_bath_time_number(bath_time_default_);
  }
  if (!std::isnan(this->bath_temperature_default_)) {
    this->set_standby_enable(true);
    this->set_bath_temperature_number(bath_temperature_default_);
  }
  if (!std::isnan(this->standby_temperature_reduction_default_)) {
    this->set_standby_temperature_reduction_number(standby_temperature_reduction_default_);
  }
  if (!std::isnan(this->humidity_step_default_)) {
    this->set_humidity_step_number(humidity_step_default_);
  }
  if (!std::isnan(this->overheating_pcb_limit_default_)) {
    this->set_overheating_pcb_limit_number(overheating_pcb_limit_default_);
  }
  if (!std::isnan(this->external_switch_renew_bathtime_default_)) {
    this->set_external_switch_renew_bathtime_number(external_switch_renew_bathtime_default_);
  }
}

void SAUNA360Component::loop() {
  while (this->available()) {
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }
  if ( (!this->tx_queue_.empty()) && (millis() - this->last_tx_ > 200)){
    send_data_();
  }
}

void SAUNA360Component::handle_char_(uint8_t c) {
  if (c == 0x98){
    this->frame_flag_ = true;
  }
  if (c == 0x98 && this->frame_flag_ == true){
    this->rx_message_.clear();
  }
  if (c == 0x9C) {
    this->rx_message_.push_back(c);
    std::vector<uint8_t> frame(this->rx_message_.begin(), this->rx_message_.end());
    size_t len = frame.size();
    if (len > 6){
      this->handle_frame_(frame);
    }
    this->rx_message_.clear();
    this->frame_flag_ = false;
    return;
  }
  if (this->frame_flag_ == true) {
    this->rx_message_.push_back(c);
  }
}

void SAUNA360Component::handle_frame_(std::vector<uint8_t> frame) {
  std::vector<uint8_t> packet;
  bool is_escaped = false;
  for (int i = 1; i < frame.size()-1; i++) {
    uint8_t d = frame[i];
    if (d==0x91){
      is_escaped = true;
      continue;
    }
    else if (is_escaped){
      is_escaped = false;
      if (d == 0x63){
        d = 0x9C; // EOF
      }
      else if (d == 0x67){
        d = 0x98; // SOF
      }
      else if (d == 0x6E){
        d = 0x91; // ESC
      }
      else{
        ESP_LOGCONFIG(TAG, "Unknown escape sequence: %02X" ,d);
        packet.push_back(0x91);
      }
    }
    packet.push_back(d);
  }
  uint16_t crc = packet[packet.size() - 1];
  crc |= (packet[packet.size() - 2]) << 8;
  packet.pop_back();
  packet.pop_back();
  uint16_t crc_calculated = crc16be(packet.data(), packet.size(), 0xffff, 0x90d9, false, false);
  if (crc == crc_calculated){
    this->handle_packet_(packet);
  }
  else {
    ESP_LOGCONFIG(TAG, "%s CRC ERROR", format_hex_pretty(packet).c_str());
  }
  frame.clear();
}

void SAUNA360Component::handle_packet_(std::vector<uint8_t> packet) {
  uint8_t address = packet[0];
  uint8_t packet_type = packet[1];
  uint16_t code = encode_uint16(packet[2],packet[3]);
  uint32_t data = encode_uint32(packet[4],packet[5],packet[6],packet[7]);
  if ((packet_type == 0x07) || (packet_type == 0x09)) {
    ESP_LOGCONFIG(TAG, "%s [ HEATER <-- PANEL ] CODE %04X DATA 0x%08X", format_hex_pretty(packet).c_str(), code, data);
    packet.clear();
    return;
  }
  ESP_LOGCONFIG(TAG, "%s [ HEATER --> PANEL ] CODE %04X DATA 0x%08X", format_hex_pretty(packet).c_str(), code, data);
  if (code == 0x1700) {
    //Facility type
    // 0x7BCB4180 also CODE 4003 DATA 0x022D0000 //Private
    // 0x7BCB4300 also CODE 4003 DATA 0x012D0000 //Time controlled 
    // 0x7BCB4600 also CODE 4003 DATA 0x002D0000 //Supervised
    // sends also code 4003 before, 2D0 is pcb temp limit
  }
  if (code == 0x3400){
    // State bits 31..0
    std::string value;
    for (auto &listener : listeners_) {listener->on_light_status((data >> 3) & 1);}
    this->light_relay_switch_->publish_state((data >> 3) & 1);
    for (auto &listener : listeners_) {listener->on_heater_status((data >> 4) & 1);}
    if ((data >> 4) & 1){
      for (auto &listener : listeners_) {listener->on_ready_status(true);}
      value = "Heating";
      for (auto &listener : listeners_) {listener->on_heater_state(value);}
    }
    else {
      value = "Off";
      for (auto &listener : listeners_) {listener->on_heater_state(value); }
    }
    if ((data >> 5) & 1){
      value = "Standby";
      for (auto &listener : listeners_) {listener->on_heater_state(value); }
    }
  }
  else if (code == 0x4002){
    int value = (data & 0xFFF);
    this->bath_time_received_hex_ = value;
    if ((value > 64) && (value < 124)) {value-=4;}
    else if ((value >= 128) && (value < 188)) {value-=8;}
    else if ((value >= 192) && (value < 252)) {value-=12;}
    else if ((value >= 256) && (value < 316)) {value-=16;}
    else if (value == 384) {value-=24;}
    else if (value >= 230) {value-=20;}
    for (auto &listener : listeners_) {listener->on_bath_time_setting(value);}
    this->bath_time_number_->publish_state(value);
    int max_bath_temperature = ((data >> 20) & 0x00FFFFF) / 18;
    this->max_bath_temperature_received_hex_ = ((data >> 20) & 0x00FFFFF);
    for (auto &listener : listeners_) {listener->on_max_bath_temperature(max_bath_temperature);}
    this->max_bath_temperature_number_->publish_state(max_bath_temperature);
  }
  else if (code == 0x4003){
    int overheating_pcb_limit = ((data >> 11) & 0x00007FF) / 18;
    this->overheating_pcb_limit_received_hex_ = ((data) & 0x007FFFFF);
    for (auto &listener : listeners_) {listener->on_overheating_pcb_limit(overheating_pcb_limit);}
    this->overheating_pcb_limit_number_->publish_state(overheating_pcb_limit);
    if (((data >> 23) & 1)) {
      this->external_switching_mode_select_->publish_state("Renew Bathtime");
    }
    else {
      this->external_switching_mode_select_->publish_state("On/Off");
    }
  }
  else if (code == 0x4004){
    this->standby_enable_switch_->publish_state((data >> 12) & 1);
    int value = (data & 0xFFF);
    this->external_switch_renew_bathtime_received_hex_ = value;
    if ((value > 64) && (value < 124)) {value-=4;}
    else if ((value >= 128) && (value < 188)) {value-=8;}
    else if ((value >= 192) && (value < 252)) {value-=12;}
    else if ((value >= 256) && (value < 316)) {value-=16;}
    else if (value == 384) {value-=24;}
    else if (value >= 230) {value-=20;}
    for (auto &listener : listeners_) {listener->on_external_switch_renew_bathtime(value);}
    this->external_switch_renew_bathtime_number_->publish_state(value);
    ESP_LOGCONFIG(TAG, "external_switch_renew_bathtime %d", value);
  }
  else if (code == 0x4200){
    ESPTime time;
    time.minute = ((data) & 0x3F);
    ESP_LOGCONFIG(TAG, "MINUTE %d", time.minute);
    time.hour = ((data >> 6) & 0x1F);
    ESP_LOGCONFIG(TAG, "HOUR %d", time.hour);
    time.day_of_month = (data >> 12) & 0x1F;
    ESP_LOGCONFIG(TAG, "DAY %d", time.day_of_month);
    time.month = (data >> 17) & 0xF;
    ESP_LOGCONFIG(TAG, "MONTH %d", time.month);
    time.year = ((data >> 21) & 0x1F) +2000;
    ESP_LOGCONFIG(TAG, "YEAR %d", time.year);
    for (auto &listener : listeners_) {listener->on_datetime(time);}
  }
  else if (code == 0x5200){
    int mode = data >> 28;
    if (mode == 0x8) { // 1000
      ESP_LOGCONFIG(TAG, "AUX0 NOT IN USE");
    }
    if (mode == 0xA) { // 1010
      ESP_LOGCONFIG(TAG, "AUX0 NOT IN USE, (ON)/OFF");
    }
    if (mode == 0xB) { // 1011
      ESP_LOGCONFIG(TAG, "AUX0 NOT IN USE, FRAGRANCE (ON)/OFF");
    }
    if (mode == 0xC) { // 1100
      ESP_LOGCONFIG(TAG, "AUX0 ON/(OFF)");
    }
    if (mode == 0xD) { // 1101
      ESP_LOGCONFIG(TAG, "AUX0 FRAGRANCE ON/(OFF)");
    }
    if (mode == 0xE) { // 1110
      ESP_LOGCONFIG(TAG, "AUX0 (ON)/OFF");
    }
    if (mode == 0xF) { // 1111
      ESP_LOGCONFIG(TAG, "AUX0 FRAGRANCE (ON)/OFF");
    }
    uint32_t stop =  (data & 0x0001FFF);
    ESP_LOGCONFIG(TAG, "AUX0 STOP %ds 1..7200s 1s..2h", stop);
    // bit13 = 0 bit14 = 1 Mystery bits, remember to add in send
    uint32_t pump =  (data >> 15 & 0x00007FF);
    ESP_LOGCONFIG(TAG, "AUX0 PUMP %dx 100ms 1..1200ms 100ms..2min", pump);
  }
  else if (code == 0x5201){
    int mode = data >> 28;
    if (mode == 0x8) { // 1000
      ESP_LOGCONFIG(TAG, "AUX1 NOT IN USE");
    }
    if (mode == 0xA) { // 1010
      ESP_LOGCONFIG(TAG, "AUX1 NOT IN USE, (ON)/OFF");
    }
    if (mode == 0xB) { // 1011
      ESP_LOGCONFIG(TAG, "AUX1 NOT IN USE, FRAGRANCE (ON)/OFF");
    }
    if (mode == 0xC) { // 1100
      ESP_LOGCONFIG(TAG, "AUX1 ON/(OFF)");
    }
    if (mode == 0xD) { // 1101
      ESP_LOGCONFIG(TAG, "AUX1 FRAGRANCE ON/(OFF)");
    }
    if (mode == 0xE) { // 1110
      ESP_LOGCONFIG(TAG, "AUX1 (ON)/OFF");
    }
    if (mode == 0xF) { // 1111
      ESP_LOGCONFIG(TAG, "AUX1 FRAGRANCE (ON)/OFF");
    }
    uint32_t stop =  (data & 0x0001FFF);
    ESP_LOGCONFIG(TAG, "AUX1 STOP %ds 1..7200s 1s..2h", stop);
    // bit13 = 0 bit14 = 1 Mystery bits, remember to add in send
    uint32_t pump =  (data >> 15 & 0x00007FF);
    ESP_LOGCONFIG(TAG, "AUX1 PUMP %dx 100ms 1..1200ms 100ms..2min", pump);
  }
  else if (code == 0x5202){
    int mode = data >> 28;
    if (mode == 0x8) { // 1000
      ESP_LOGCONFIG(TAG, "AUX2 NOT IN USE");
    }
    if (mode == 0xA) { // 1010
      ESP_LOGCONFIG(TAG, "AUX2 NOT IN USE, (ON)/OFF");
    }
    if (mode == 0xB) { // 1011
      ESP_LOGCONFIG(TAG, "AUX2 NOT IN USE, FRAGRANCE (ON)/OFF");
    }
    if (mode == 0xC) { // 1100
      ESP_LOGCONFIG(TAG, "AUX2 ON/(OFF)");
    }
    if (mode == 0xD) { // 1101
      ESP_LOGCONFIG(TAG, "AUX2 FRAGRANCE ON/(OFF)");
    }
    if (mode == 0xE) { // 1110
      ESP_LOGCONFIG(TAG, "AUX2 (ON)/OFF");
    }
    if (mode == 0xF) { // 1111
      ESP_LOGCONFIG(TAG, "AUX2 FRAGRANCE (ON)/OFF");
    }
    uint32_t stop =  (data & 0x0001FFF);
    ESP_LOGCONFIG(TAG, "AUX2 STOP %ds 1..7200s 1s..2h", stop);
    // bit13 = 0 bit14 = 1 Mystery bits, remember to add in send
    uint32_t pump =  (data >> 15 & 0x00007FF);
    ESP_LOGCONFIG(TAG, "AUX2 PUMP %dx 100ms 1..1200ms 100ms..2min", pump);
  }
  else if (code == 0x6000){
    int actual_temp = (data & 0x00007FF) / 9.0;
    this->temperature_received_hex_ = (data & 0x00007FF);
    for (auto &listener : listeners_) {listener->on_temperature(actual_temp);}
    int setpoint_temp = ((data >> 11) & 0x00007FF) / 9.0;
    this->setpoint_temperature_received_hex_ = ((data >> 11) & 0x00007FF);
    for (auto &listener : listeners_) {listener->on_temperature_setting(setpoint_temp);}
    this->bath_temperature_number_->publish_state(setpoint_temp);
    int standby_temperature_reduction = ((data >> 21) & 0x007FFFF) / 18;
    this->standby_temperature_reduction_received_hex_ = ((data >> 21) & 0x007FFFF);
    for (auto &listener : listeners_) {listener->on_standby_temperature_reduction(standby_temperature_reduction);}
    this->standby_temperature_reduction_number_->publish_state(standby_temperature_reduction);
  }
  else if (code == 0x6001) {
    int humidity_setting = (((data >> 4) & 0x00000FF) - 40) / 8;
    for (auto &listener : listeners_) {listener->on_humidity(humidity_setting);}
    this->humidity_step_number_->publish_state(humidity_setting);
    this->bath_type_priority_received_hex_ = ((data) & 0x000F000);
    if (((data) & 0x0000F000) == 0x3000){
      this->bath_type_priority_select_->publish_state("Automatic");
    }
    if (((data) & 0x0000F000) == 0x7000){
      this->bath_type_priority_select_->publish_state("Temperature");
    }
    if (((data) & 0x0000F000) == 0xB000){
      this->bath_type_priority_select_->publish_state("Humidity");
    }
    //for (auto &listener : listeners_) {listener->on_humidity_percentage(humidity_percentage);}
    //humidity percentage might also be in this code? cant really test until have combi elite Rh% sensor. 
  }
  else if (code == 0x7000){
    //command acknowledge
    if ((data == 0x00400000)){
      std::string value = "Operation blocked by not allowed start";
      for (auto &listener : listeners_) {listener->on_heater_state(value);}
    }
  }
  else if (code == 0x7180) {
    for (auto &listener : listeners_) {listener->on_relay_x3_x4_status((data >> 0) & 1);}
    for (auto &listener : listeners_) {listener->on_relay_x5_x6_status((data >> 1) & 1);}
    for (auto &listener : listeners_) {listener->on_relay_x7_x8_status((data >> 2) & 1);}
    for (auto &listener : listeners_) {listener->on_relay_x9_x10_status((data >> 3) & 1);}
    for (auto &listener : listeners_) {listener->on_relay_x11_x12_status((data >> 4) & 1);}
    for (auto &listener : listeners_) {listener->on_relay_x13_x14_status((data >> 5) & 1);}
    for (auto &listener : listeners_) {listener->on_relay_x15_x16_status((data >> 6) & 1);}
    for (auto &listener : listeners_) {listener->on_relay_x17_x18_status((data >> 7) & 1);}
    this->light_relay_switch_->publish_state((data >> 5) & 1);
    this->aux0_relay_switch_->publish_state((data >> 6) & 1);
    this->aux1_relay_switch_->publish_state((data >> 4) & 1);
    this->aux2_relay_switch_->publish_state((data >> 3) & 1);
  }
  else if (code == 0x7280){
    std::string value;
    if (data == 0){
      value = "Low";
    }
    else if (data == 0x800) {
      value = "Half";
    }
    else if (data == 0x1000) {
      value = "Full";
    }
    else if (data == 0x1800) {
      value = "Error, Level electrodes steam tank out of service, interruption mid";
    }
    else {
      value = "Error unknown, Steam tank out of service";
    }
    for (auto &listener : listeners_) {listener->on_water_level(value);}
  }
  else if (code == 0x9000){
    this->activate_time_limit_switch_->publish_state((data >> 22) & 1);
    this->activate_time_limit_ = ((data >> 22) & 1);
    ESPTime time_from;
    time_from.minute = ((data) & 0x3F);
    time_from.hour = ((data >> 6) & 0x1F);
    for (auto &listener : listeners_) {listener-> on_not_allowed_start_from_time(time_from);}
    this->time_limit_from_ = time_from;
    ESP_LOGCONFIG(TAG, "FROM %d:%d", time_from.hour, time_from.minute);
    ESPTime time_until;
    time_until.minute = ((data >> 11) & 0x3F);
    time_until.hour = ((data >> 17) & 0x1F);
    for (auto &listener : listeners_) {listener-> on_not_allowed_start_until_time(time_until);}
    this->time_limit_until_ = time_until;
    ESP_LOGCONFIG(TAG, "UNTIL %d:%d", time_until.hour, time_until.minute);
  }
  else if (code == 0x9400){
    for (auto &listener : listeners_) {listener->on_total_uptime(data);}
  }
  else if (code == 0x9401){
    for (auto &listener : listeners_) {listener->on_remaining_time(data);}
  }
  else if (code == 0xB000){
    for (auto &listener : listeners_) {listener->on_ready_status((data) & 1);}
    if (data == 0x00060001){
      std::string value = "Operation blocked by not allowed start";
      for (auto &listener : listeners_) {listener->on_heater_state(value);}
      this->create_send_data_(0x07, 0xB000, 0x00060101);
    }
  }
  else {
    ESP_LOGCONFIG(TAG, "^^^^^^^^^^^^^^^^^^^^^^^ PACKET NOT HANDLED YET ");
  }
  packet.clear();
}

void SAUNA360Component::set_heater_on() {
  this->create_send_data_(0x07, 0x7000, 0x00000040);
}

void SAUNA360Component::set_heater_off() {
  this->create_send_data_(0x07, 0x7000, 0x00000080);
  }

void SAUNA360Component::set_heater_standby() {
  this->create_send_data_(0x07, 0x7000, 0x004000C0);
  }

void SAUNA360Component::set_heater_power_toggle() { 
  this->create_send_data_(0x07, 0x7000, 0x00000001);
  }

void SAUNA360Component::set_bath_time_number(float value) {
  if ((value > 60) && (value < 120)) {value+=4;}
  else if ((value >= 120) && (value < 180)) {value+=8;}
  else if ((value >= 180) && (value < 240)) {value+=12;}
  else if ((value >= 240) && (value < 300)) {value+=16;}
  else if (value >= 300) {value+=20;}
  uint32_t data = ((uint32_t) value);
  if(this->max_bath_temperature_received_hex_) {
    data |= (this->max_bath_temperature_received_hex_ << 20);
  }
  else {
    data |= (((uint32_t) this->max_bath_temperature_default_ * 18 ) << 20);
  }
  this->create_send_data_(0x07, 0x4002, data);
}

void SAUNA360Component::set_max_bath_temperature_number(float value) {
  uint32_t data = this->bath_time_received_hex_; 
  data |= (((uint32_t) value * 18 ) << 20);
  this->create_send_data_(0x07, 0x4002, data);
}

void SAUNA360Component::set_bath_temperature_number(float value) {
  uint32_t data = ((this->standby_temperature_reduction_received_hex_) << 21);
  data |= (((uint32_t) value * 9 ) << 11);
  data |= this->temperature_received_hex_;
  this->create_send_data_(0x07, 0x6000, data);
}

void SAUNA360Component::set_standby_temperature_reduction_number(float value) {
  uint32_t data = (((uint32_t) value * 18 ) << 21);
  if(this->setpoint_temperature_received_hex_) {
    data |= ((this->setpoint_temperature_received_hex_) << 11);
  }
  else {
    data |= (((uint32_t) this->bath_temperature_default_ * 9 ) << 11);
  }
  data |= ((this->setpoint_temperature_received_hex_) << 11);
  data |= this->temperature_received_hex_;
  this->create_send_data_(0x07, 0x6000, data);
}

void SAUNA360Component::set_humidity_step_number(float value) {
  uint32_t data = this->bath_type_priority_received_hex_;
  data |= (((uint32_t) value * 8 + 40 ) << 4);
  this->create_send_data_(0x07, 0x6001, data);
}

void SAUNA360Component::set_overheating_pcb_limit_number(float value) {
  uint32_t data = (((uint32_t) value * 18) << 11);
  auto index = this->external_switching_mode_select_->active_index();
  switch (index.value()) {
    case 0: //On/Off
      data |= 0 << 23;
      break;
    case 1: //Renew Bathtime 
      data |= 1 << 23;
      break;
  }
  this->create_send_data_(0x07, 0x4003, data);
}

void SAUNA360Component::set_external_switch_renew_bathtime_number(float value) {
  if ((value > 60) && (value < 120)) {value+=4;}
  else if ((value >= 120) && (value < 180)) {value+=8;}
  else if ((value >= 180) && (value < 240)) {value+=12;}
  else if ((value >= 240) && (value < 300)) {value+=16;}
  else if (value >= 300) {value+=20;}
  uint32_t data = ((uint32_t) value);
  data |= (this->standby_enable_switch_) ? (1 << 12) : (0 << 12) ;
  this->create_send_data_(0x07, 0x4004, data);
}

void SAUNA360Component::set_light_relay(bool enable) {
  this->create_send_data_(0x07, 0x7000, 0x2);
}

void SAUNA360Component::set_aux0_relay(bool enable) {
  uint32_t data = (enable) ? 0xE000A4B0 : 0xC000A4B0;
  this->create_send_data_(0x07, 0x5200, data);
}

void SAUNA360Component::set_aux0_relay_mode(const std::string &state) {
  uint32_t data;
  auto index = this->aux0_relay_mode_select_->active_index();
  switch (index.value()) {
    case 0: //Not in use
      data = 0x8000000;
      break;
    case 1: //On/Off 
      data = 0xC0000000;
      break;
    case 2: //Fragrance 
      data = 0xD0000000;
      break;
    //pump 100ms...2min
    //stop 1s...2h
    // 0x5000A001 pump 100ms stop 1s
    // 0x52583C20 pump 2min 2h
  }
  //this->create_send_data_(0x07, 0x5200, data);
  //not yet in use
}

void SAUNA360Component::set_aux1_relay_mode(const std::string &state) {
  uint32_t data;
  auto index = this->aux1_relay_mode_select_->active_index();
  switch (index.value()) {
    case 0: //Not in use
      data = 0x8000000;
      break;
    case 1: //On/Off 
      data = 0xC0000000;
      break;
    case 2: //Fragrance 
      data = 0xD0000000;
      break;
    //pump 100ms...2min
    //stop 1s...2h
    // 0x5000A001 pump 100ms stop 1s
    // 0x52583C20 pump 2min 2h
  }
  //this->create_send_data_(0x07, 0x5201, data);
  //not yet in use
}

void SAUNA360Component::set_aux2_relay_mode(const std::string &state) {
  uint32_t data;
  auto index = this->aux2_relay_mode_select_->active_index();
  switch (index.value()) {
    case 0: //Not in use
      data = 0x8000000;
      break;
    case 1: //On/Off 
      data = 0xC0000000;
      break;
    case 2: //Fragrance 
      data = 0xD0000000;
      break;
      //pump 100ms...2min
      //stop 1s...2h
      // 0x5000A001 pump 100ms stop 1s
      // 0x52583C20 pump 2min 2h
  }
  //this->create_send_data_(0x07, 0x5202, data);
  //not yet in use
}

void SAUNA360Component::set_external_switching_mode(const std::string &state) {
  uint32_t data = overheating_pcb_limit_received_hex_;
  auto index = this->external_switching_mode_select_->active_index();
  switch (index.value()) {
    case 0: //On/Off
      data |= 0 << 23;
      break;
    case 1: //Renew Bathtime 
      data |= 1 << 23;
      break;
  }
  this->create_send_data_(0x07, 0x4003, data);
}

void SAUNA360Component::set_bath_type_priority(const std::string &state) {
  uint32_t data;
  auto index = this->bath_type_priority_select_->active_index();
  switch (index.value()) {
    case 0: //Automatic
      data |= 0x3 << 12;
      break;
    case 1: //Temperature 
      data |= 0x7 << 12;
      break;
    case 2: //Humidity
      data |= 0xB << 12;
      break;
  }
  this->create_send_data_(0x07, 0x6001, data);
}


void SAUNA360Component::set_aux1_relay(bool enable) {
  uint32_t data = (enable) ? 0xE000A4B0 : 0xC000A4B0;
  this->create_send_data_(0x07, 0x5201, data);
}

void SAUNA360Component::set_aux2_relay(bool enable) {
  uint32_t data = (enable) ? 0xE000A4B0 : 0xC000A4B0;
  this->create_send_data_(0x07, 0x5202, data);
}

void SAUNA360Component::set_standby_enable(bool enable) {
  uint32_t data = (enable) ? (1 << 12) : (0 << 12);
  data |= this->external_switch_renew_bathtime_received_hex_;
  this->create_send_data_(0x07, 0x4004, data);
}

void SAUNA360Component::set_datetime(ESPTime &time) {
  uint32_t data = (time.minute);
  data |= ((uint32_t) time.hour << 6);
  data |= (1 << 11);
  data |= ((uint32_t) time.day_of_month << 12);
  data |= ((uint32_t) time.month << 17);
  data |= (((uint32_t) time.year -2000) << 21);
  data |= (1 << 31);
  this->create_send_data_(0x07, 0x4200, data);
}

void SAUNA360Component::set_not_allowed_start_from_time(ESPTime &time) {
  uint32_t data = ((uint32_t) time.minute);
  data |= ((uint32_t) time.hour << 6);
  data |= ((uint32_t) this->time_limit_until_.minute << 11);
  data |= ((uint32_t) this->time_limit_until_.hour << 17);
  data |= (this->activate_time_limit_) ? (1 << 22) : (0 << 22);
  this->create_send_data_(0x07, 0x9000, data);
}

void SAUNA360Component::set_not_allowed_start_until_time(ESPTime &time) {
  uint32_t data = ((uint32_t) this->time_limit_from_.minute);
  data |= ((uint32_t) this->time_limit_from_.hour << 6);
  data |= ((uint32_t) time.minute << 11);
  data |= ((uint32_t) time.hour << 17);
  data |= (this->activate_time_limit_) ? (1 << 22) : (0 << 22);
  this->create_send_data_(0x07, 0x9000, data);
}

void SAUNA360Component::set_activate_time_limit(bool enable) {
  uint32_t data = ((uint32_t) this->time_limit_from_.minute);
  data |= ((uint32_t) this->time_limit_from_.hour << 6);
  data |= ((uint32_t) this->time_limit_until_.minute << 11);
  data |= ((uint32_t) this->time_limit_until_.hour << 17);
  data |= (enable) ? (1 << 22) : (0 << 22);
  this->create_send_data_(0x07, 0x9000, data);
}

void SAUNA360Component::create_send_data_(uint8_t type, uint16_t code, uint32_t data) {
  ESP_LOGCONFIG(TAG, "CREATING SEND DATA TYPE:%s CODE:%s DATA:%s", format_hex_pretty(type).c_str(), format_hex_pretty(code).c_str(), format_hex_pretty(data).c_str());
  std::vector<uint8_t> packet;
  uint8_t id = 0x40;
  packet.push_back(id);
  packet.push_back(type);
  std::array<uint8_t, 2> code_array = decode_value(code);
  packet.push_back(code_array[0]);
  packet.push_back(code_array[1]);
  std::array<uint8_t, 4> data_array = decode_value(data);
  packet.push_back(data_array[0]);
  packet.push_back(data_array[1]);
  packet.push_back(data_array[2]);
  packet.push_back(data_array[3]);
  uint16_t crc_calculated = crc16be(packet.data(), packet.size(), 0xffff, 0x90d9, false, false);
  std::array<uint8_t, 2> crc_array = decode_value(crc_calculated);
  packet.push_back(crc_array[0]);
  packet.push_back(crc_array[1]);
  uint8_t eof = 0x9C;
  uint8_t eof_esc = 0x63;
  uint8_t sof = 0x98;
  uint8_t sof_esc = 0x67;
  uint8_t esc = 0x91;
  uint8_t esc_esc = 0x6E;
  std::vector<uint8_t> packet_escaped;
  for (int i = 0; i < sizeof(packet)-2; i++) {
    uint8_t d = ((uint8_t) packet[i]);
    if (d==sof){
      packet_escaped.push_back(esc);
      packet_escaped.push_back(sof_esc);
    }
    else if (d==eof){
      packet_escaped.push_back(esc);
      packet_escaped.push_back(eof_esc);
    }
    else if (d==esc){
      packet_escaped.push_back(esc);
      packet_escaped.push_back(esc_esc);
    }
    else{
      packet_escaped.push_back(d);
    }
  }
  packet_escaped.insert(packet_escaped.begin(),sof);
  packet_escaped.push_back(eof);
  ESP_LOGCONFIG(TAG, "%s CREATED PACKET" ,format_hex_pretty(packet_escaped).c_str());
  this->tx_queue_.push(packet_escaped);
  packet.clear();
  packet_escaped.clear();
}

void SAUNA360Component::send_data_() {
  if (!this->tx_queue_.empty()) {
    auto packet = std::move(this->tx_queue_.front());
    this->tx_queue_.pop();
    ESP_LOGCONFIG(TAG, "%s SENDING FROM TX QUEUE:", format_hex_pretty(packet).c_str());
    if (this->flow_control_pin_ != nullptr) {
      ESP_LOGCONFIG(TAG, "FLOW CONTROL ON");
    }
    for (int i = 0; i < 40; i++) {
      this->write_byte(0x00);
    }
    this->write_array(packet);
    this->flush();
    this->last_tx_ = millis();
    if (this->flow_control_pin_ != nullptr) {
      this->flow_control_pin_->digital_write(false);
      ESP_LOGCONFIG(TAG, "FLOW CONTROL OFF"); 
    }
  ESP_LOGCONFIG(TAG, "DATA SENT SUCCESFULLY");
  }
}

void SAUNA360Component::dump_config(){
    ESP_LOGCONFIG(TAG, "UART component");
    LOG_PIN("Flow Control Pin:", this->flow_control_pin_);
    ESP_LOGCONFIG(TAG, "Default Bath Time: %f", bath_time_default_);
}

}  // namespace sauna360
}  // namespace esphome