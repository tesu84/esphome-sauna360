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
    this->frame_flag_ = true;
    this->rx_message_.clear();
  }

  if (c == 0x9C) {

    std::vector<uint8_t> frame(this->rx_message_.begin(), this->rx_message_.end());

    if ((frame[4] != 0xE3) && (frame[4] != 0x3A )) {
      ESP_LOGCONFIG(TAG, "%s FRAME RECEIVED", format_hex_pretty(frame).c_str());
    }
    
    this->handle_frame_(frame);
    this->rx_message_.clear();
    this->frame_flag_ = false;
    return;
  }
  if (this->frame_flag_ == true) {
    this->rx_message_.push_back(c);
  }
}

 void SAUNA360Component::send_data_() {

    // Send the next packet in the queue
    if (!this->tx_queue_.empty()) {
      auto packet = std::move(this->tx_queue_.front());
      this->tx_queue_.pop();
      ESP_LOGCONFIG(TAG, "%s SENDING FROM TX QUEUE:", format_hex_pretty(packet).c_str());

      if (this->flow_control_pin_ != nullptr) {
        ESP_LOGCONFIG(TAG, "FLOW CONTROL ON");
      }
      
      //reserve bus for sending
      for (int i = 1; i < 40; i++) {
        write_byte(0x00);
      }

      this->write_array(packet);
      this->flush();
      this->last_tx_ = millis();

      if (this->flow_control_pin_ != nullptr) {
        this->flow_control_pin_->digital_write(false);
        ESP_LOGCONFIG(TAG, "FLOW CONTROL OFF"); 
      }
    }
    ESP_LOGCONFIG(TAG, "DATA SENT SUCCESFULLY %zuus", micros()-last_tx_);
  }

void SAUNA360Component::handle_frame_(std::vector<uint8_t> frame) {

  std::vector<uint8_t> packet;
  bool isEscaped = false;
  for (int i = 1; i < frame.size()-1; i++) {
    uint8_t d = frame[i];
    if (d==0x91){
      isEscaped = true;
      continue;
    }
    else if (isEscaped){
      isEscaped = false;
      if (d == 0x63){
        d = 0x9c; // EOF
      }
      else if (d == 0x67){
        d = 0x98; // SOF
      }
      else if (d == 0x6E){
        d = 0x91; // ESC
      }
      else{
        ESP_LOGCONFIG(TAG, "Unknown escape sequence: %02x" ,d);
        packet.push_back(0x91);
      }
    }
    packet.push_back(d);
  }

   if (crc16be(packet.data(), packet.size(), 0xffff, 0x90d9, false, false)){
     packet.pop_back();
     this->handle_packet_(packet);
   }

   else {
    ESP_LOGCONFIG(TAG, "%s CRC ERROR", format_hex_pretty(frame).c_str());
   }
   frame.clear();
}

void SAUNA360Component::handle_packet_(std::vector<uint8_t> packet) {
  //skip keepalive handshakes
  size_t len = packet.size();
  if (len <= 2){
    packet.clear();
    return;
  }

  uint8_t address = packet[0];
  uint8_t packetType = packet[1];
  uint16_t code = encode_uint16(packet[2],packet[3]);
  uint32_t data = encode_uint32(packet[4],packet[5],packet[6],packet[7]);

  //Discard packets from panel to heater
  if ((packetType == 0x07) || (packetType == 0x09)) {
    packet.clear();
    return;
  }

  if (code == 0x6000){
    // temperature data point. Split into set point and actual value
    int actual_temp = (data & 0x00007FF) / 9.0;
    this->temperature_received_hex_ = (data & 0x00007FF);
    for (auto &listener : listeners_) {listener->on_temperature(actual_temp);}
    int setpoint_temp = ((data >> 11) & 0x00007FF) / 9.0;
    for (auto &listener : listeners_) {listener->on_temperature_setting(setpoint_temp);}
  }

  else if (code == 0x3400){
    /* status code
    # 3400 bbbbbbbb - State bits, sent between on/off command and acknowledge, known values so far:
    #      00000001 - light off / heater off
    #      00000009 - light on / heater off
    #      00000011 - light off / heater on
    #      00000019 - light on / heater on */
    if ((data & 0x00000008) != 0) {
      for (auto &listener : listeners_) {listener->on_light_status(true);}
    }
    else {
      for (auto &listener : listeners_) {listener->on_light_status(false);}
    }
    if ((data & 0x00000010) != 0) {
      for (auto &listener : listeners_) {listener->on_heater_status(true);}
      //set ready status true on here too so it gets state faster, if sauna is ready during boot.
      for (auto &listener : listeners_) {listener->on_ready_status(true);}
    }
    else {
      for (auto &listener : listeners_) {listener->on_heater_status(false);}
    }
  }

  else if (code == 0xb000){
    // Ready status, if door is open too long status = false
    // After door closed and ACK from panel pressing power button,
    // status = true and sauna can be started.
    // safety feature by manufacturer so user have to check sauna visually.
    if (data == 0x00140000) {
      for (auto &listener : listeners_) {listener->on_ready_status(true);}
    }
    else if (data == 0x00140003) {
      for (auto &listener : listeners_) {listener->on_ready_status(false);}
    }
  }

  else if (code == 0x9400){
    //total operation time minute upcounter.
    ESP_LOGCONFIG(TAG, "Uptime: %dmin" ,data);
  }

  else if (code == 0x9401){
    //bathing time minute downcounter.
    for (auto &listener : listeners_) {listener->on_remaining_time(data);}
  }

  else if (code == 0x4002){
    //  bath time
     int value = (data & 0xFFF);
      if ((value > 64) && (value < 124)) {value-=4;}
      else if ((value >= 128) && (value < 188)) {value-=8;}
      else if ((value >= 192) && (value < 252)) {value-=12;}
      else if ((value >= 256) && (value < 316)) {value-=16;}
      else if (value >= 230) {value-=20;}

      for (auto &listener : listeners_) {listener->on_bath_time_setting(value);}
  }

  else if (code == 0x4200){
    //  probably clock
  }

  else if (code == 0x6001) {
    // Extract the second byte and the lower nibble of the third byte
    uint8_t second_byte = (data & 0xFF00) >> 8;
    uint8_t third_byte_lower_nibble = (data & 0x00F0) >> 4;
    // Calculate the humidity setting based on the pattern and adjust by subtracting 5
    uint16_t humidity_setting = (second_byte - 0x30) * 2 + (third_byte_lower_nibble / 8) - 5;
    ESP_LOGCONFIG(TAG, "Humidity setting: %d", humidity_setting);
    for (auto &listener : listeners_) {listener->on_humidity(humidity_setting);}
  }

  else if (code == 0x7180) {
    // Extract humidity percentage from the data
    uint16_t humidity_percentage = data & 0xFFFF; // Assuming the humidity is in the least significant byte
    ESP_LOGCONFIG(TAG, "Humidity percentage: %d%%", humidity_percentage);
    for (auto &listener : listeners_) {listener->on_humidity_percentage(humidity_percentage);}
  }

  else {
    ESP_LOGCONFIG(TAG, "%s PACKET NOT HANDLED. CODE %s DATA %s" , format_hex_pretty(packet).c_str(), format_hex_pretty(code).c_str(), format_hex_pretty(data).c_str());
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
  std::vector<uint8_t> packet_escaped;

  uint8_t id = 0x40;
  uint8_t eof = 0x9C;
  uint8_t eof_esc = 0x63;
  uint8_t sof = 0x98;
  uint8_t sof_esc = 0x67;
  uint8_t esc = 0x91;

  std::array<uint8_t, 2> code_array = decode_value(code);
  std::array<uint8_t, 4> data_array = decode_value(data);

  packet.push_back(id);
  packet.push_back(type);
  packet.push_back(code_array[0]);
  packet.push_back(code_array[1]);
  packet.push_back(data_array[0]);
  packet.push_back(data_array[1]);
  packet.push_back(data_array[2]);
  packet.push_back(data_array[3]);

  uint16_t crc_calculated = crc16be(packet.data(), packet.size(), 0xffff, 0x90d9, false, false);
  std::array<uint8_t, 2> crc_array = decode_value(crc_calculated);
  packet.push_back(crc_array[0]);
  packet.push_back(crc_array[1]);

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

void SAUNA360Component::dump_config(){
    ESP_LOGCONFIG(TAG, "UART component");
    LOG_PIN("Flow Control Pin:", this->flow_control_pin_);
    ESP_LOGCONFIG(TAG, "Default Bath Time: %f", bath_time_default_);
}

}  // namespace sauna360
}  // namespace esphome