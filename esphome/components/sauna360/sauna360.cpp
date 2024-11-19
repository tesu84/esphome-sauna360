#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/components/uart/uart_component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
#include "esphome/core/preferences.h"
#include "esphome/core/automation.h"
#include "sauna360.h"


namespace esphome {
namespace sauna360 {

static const char *TAG = "sauna360";

void SAUNA360Component::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
  if (!std::isnan(this->bath_time_default_)) {
    this->bath_time_number_->publish_state(bath_time_default_);
  }
  if (!std::isnan(this->bath_temperature_default_)) {
    this->bath_temperature_number_->publish_state(bath_temperature_default_);
  }
}

void SAUNA360Component::loop() {
  while (this->available()) {
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }
  if ( (!this->tx_queue_.empty()) && (millis() - this->last_tx_ > 250)){
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
  ESP_LOGCONFIG(TAG, "%s DEBUG WITH ESCAPE", format_hex_pretty(frame).c_str());
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
  ESP_LOGCONFIG(TAG, "%s DEBUG AFTER ESCAPE", format_hex_pretty(packet).c_str());
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
  uint8_t packetType = packet[1];
  uint16_t code = encode_uint16(packet[2],packet[3]);
  uint32_t data = encode_uint32(packet[4],packet[5],packet[6],packet[7]);
  if ((packetType == 0x07) || (packetType == 0x09)) {
    ESP_LOGCONFIG(TAG, "%s [ HEATER <-- PANEL ] CODE %04X DATA 0x%08X", format_hex_pretty(packet).c_str(), code, data);
    packet.clear();
    return;
  }
  ESP_LOGCONFIG(TAG, "%s [ HEATER --> PANEL ] CODE %04X DATA 0x%08X", format_hex_pretty(packet).c_str(), code, data);
  if (code == 0x3400){
    // State bits 31..0
    for (auto &listener : listeners_) {listener->on_light_status((data >> 3) & 1);}
    for (auto &listener : listeners_) {listener->on_heater_status((data >> 4) & 1);}
    for (auto &listener : listeners_) {listener->on_ready_status((data >> 4) & 1);}
    // set ready_status true here also if heater is ready after boot so it gets value faster.
  }
  else if (code == 0x4002){
    int value = (data & 0xFFF);
    if ((value > 64) && (value < 124)) {value-=4;}
    else if ((value >= 128) && (value < 188)) {value-=8;}
    else if ((value >= 192) && (value < 252)) {value-=12;}
    else if ((value >= 256) && (value < 316)) {value-=16;}
    else if (value >= 230) {value-=20;}
    for (auto &listener : listeners_) {listener->on_bath_time_setting(value);}
    //max bath temp 40-110
    //100 70.8B.40.5A
    //101 71.AB.40.5A
    //102 72.CB.40.5A
    //103 73.EB.40.5A
    //104 75.0B.40.5A
  }
  else if (code == 0x4003){
    //Overheating PCB limit 70-90 default 80
    //81 06.2D.93.DE
    //82 06.2E.23.DE
    //83 06.2E.B3.DE
    //
    //External switch function
    //Renew Bathtime 06.AE.B3.DE overheat was 83
    //On/Off         06.2E.B3.DE overheat was 83
  }
  else if (code == 0x4004){
    //Standby
    //disable 00.00.00.1E
    //enable  00.00.10.1E
    //
    //External switch 
    //Bath time 1-360min
    //code 4004
    //1    00.00.10.01
    //2    00.00.10.02
    //360  00.00.11.80
  }
  else if (code == 0x4200){
    //Time/Date
    //20:12 17/11/2024 83.17.1D.0C
    //20:13 17/11/2024 83.17.1D.0D
    //20:14 17/11/2024 83.17.1D.0E
    //20:15 17/11/2024 83.17.1D.0F
  }
  else if (code == 0x5200){
    //Aux 0 Relay 6 (FAN) IN WE30
    //Options:
    //Not in use 80.00.A4.B0
    //On/off     C0.00.A4.B0
    //Fragrance  D0.00.A4.B0
    //data:
    //On  E0.00.A4.B0
    //Off C0.00.A4.B0
  }
  else if (code == 0x5201){
    //Aux 1 Relay 7
    //Options:
    //Not in use 80.00.A4.B0
    //On/off     C0.00.A4.B0
    //Fragrance  D0.00.A4.B0
    //data:
    //On  E0.00.A4.B0
    //Off C0.00.A4.B0
  }
  else if (code == 0x5202){
    //Aux 2
    //Options:
    //Not in use 80.00.A4.B0
    //On/off     C0.00.A4.B0
    //Fragrance  D0.00.A4.B0
    //data:
    //On  E0.00.A4.B0
    //Off C0.00.A4.B0
  }
  else if (code == 0x6000){
    // temperature data point. Split into set point and actual value
    int actual_temp = (data & 0x00007FF) / 9.0;
    this->temperature_received_hex_ = (data & 0x00007FF);
    for (auto &listener : listeners_) {listener->on_temperature(actual_temp);}
    int setpoint_temp = ((data >> 11) & 0x00007FF) / 9.0;
    for (auto &listener : listeners_) {listener->on_temperature_setting(setpoint_temp);}
    //Standby temp reduction 0-30
    //0 00.0B.40.E1
    //1 02.4B.40.E1
    //2 04.8B.40.E1
    //3 06.CB.40.E1
  }
  else if (code == 0x6001) {
    // Extract the second byte and the lower nibble of the third byte
    uint8_t second_byte = (data & 0xFF00) >> 8;
    uint8_t third_byte_lower_nibble = (data & 0x00F0) >> 4;
    // Calculate the humidity setting based on the pattern and adjust by subtracting 5
    uint16_t humidity_setting = (second_byte - 0x30) * 2 + (third_byte_lower_nibble / 8) - 5;
    ESP_LOGCONFIG(TAG, "Humidity setting: %d", humidity_setting);
    for (auto &listener : listeners_) {listener->on_humidity(humidity_setting);}
    //for (auto &listener : listeners_) {listener->on_humidity_percentage(humidity_percentage);}
  }
  else if (code == 0x7000){
    //command acknowledge
    //ght toggle
    //de 7000 
    //data 00000002
  }
  else if (code == 0x7180) {
    //State bits 31..00
    //ESP_LOGCONFIG(TAG, "Relay 1 X3-X4 %d", ((data >> 0) & 1));
    //ESP_LOGCONFIG(TAG, "Relay 2 X5-X6 %d", ((data >> 1) & 1));
    //ESP_LOGCONFIG(TAG, "Relay 3 X7-X8 %d", ((data >> 2) & 1));
    //ESP_LOGCONFIG(TAG, "Relay 4 X9-X10 %d", ((data >> 3) & 1));
    //ESP_LOGCONFIG(TAG, "Relay 5 X11-X12 %d", ((data >> 4) & 1));
    //ESP_LOGCONFIG(TAG, "Relay 6 X13-X14 %d", ((data >> 5) & 1));
    //ESP_LOGCONFIG(TAG, "Relay 7 X15-X16 %d", ((data >> 6) & 1));
    //ESP_LOGCONFIG(TAG, "Relay 8 X17-X18 %d", ((data >> 7) & 1));
  }
  else if (code == 0x7280){
    //Water level
    //CODE 7280 DATA 0x00000000 LOW
    //CODE 7280 DATA 0x00000800 MID
    //CODE 7280 DATA 0x00001000 HIGH
  }
  else if (code == 0x9000){
    //Not allowed start 00:00 - 23:59 (1440min)
    //Enabled  from 00:00 Until 00:00  00.40.00.00
    //Disabled from 00:00 Until 00:00  00.00.00.00
    //Enabled  from 00:01 until 00:00  00.40.00.01
    //Disabled from 00:01 until 00:00  00.00.00.01
    //Enabled  from 00:00 until 00:01  00.40.08.00
    //Disabled from 00:00 until 00:01  00.00.08.00
    //Enabled  from 23:59 until 23:59  00.6F.DD.FB
    //Disabled from 23:59 until 23:59  00.2F.DD.FB
  }
  else if (code == 0x9400){
    //total operation time minute upcounter.
  }
  else if (code == 0x9401){
    for (auto &listener : listeners_) {listener->on_remaining_time(data);}
  }
  else if (code == 0xB000){
    for (auto &listener : listeners_) {listener->on_ready_status((data) & 1);}
  }
  else {
    ESP_LOGCONFIG(TAG, "^^^^^^^^^^^^^^^^^^^^^^^ PACKET NOT HANDLED YET ");
  }
  packet.clear();
}

void SAUNA360Component::apply_heater_on_action() {
  this->create_send_data_(0x07, 0x7000, 0x00000040);
  ESP_LOGCONFIG(TAG, "SETTING HEATER ON");
    return;
}

void SAUNA360Component::apply_heater_off_action() {
  this->create_send_data_(0x07, 0x7000, 0x00000080);
  ESP_LOGCONFIG(TAG, "SETTING HEATER OFF");
    return;
  }

void SAUNA360Component::apply_heater_standby_action() {
  this->create_send_data_(0x07, 0x7000, 0x004000C0);
  ESP_LOGCONFIG(TAG, "SETTING HEATER STANDBY");
    return;
  }

void SAUNA360Component::apply_heater_power_toggle_action() { 
  this->create_send_data_(0x07, 0x7000, 0x00000001);
  ESP_LOGCONFIG(TAG, "POWER TOGGLE");
    return;
  }

void SAUNA360Component::set_bath_time_number(float value) {
  if ((value > 60) && (value < 120)) {value+=4;}
  else if ((value >= 120) && (value < 180)) {value+=8;}
  else if ((value >= 180) && (value < 240)) {value+=12;}
  else if ((value >= 240) && (value < 300)) {value+=16;}
  else if (value >= 300) {value+=20;}
  uint32_t data = ((uint32_t) value);
  data |= (((uint32_t) 0x7BCB4) << 12);
  this->create_send_data_(0x07, 0x4002, data);
}

void SAUNA360Component::set_bath_temperature_number(float value) {
  uint32_t data = (((uint32_t) value * 9 ) << 11);
  data |= this->temperature_received_hex_;
  this->create_send_data_(0x07, 0x6000, data);
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