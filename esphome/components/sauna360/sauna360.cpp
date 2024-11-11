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
  const uint32_t now = micros();

  while (this->available()) {
    this->last_rx_ = now;
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }

  //wait bus silence x char time before sending data
  if (this->rx_message_.empty() && (now - this->last_rx_ > 2000)){
    send_data_();
  } 

}

void SAUNA360Component::handle_char_(uint8_t c) {
  const uint32_t now_micros = micros(); //for debug
  //ESP_LOGCONFIG(TAG, "C %02x" , c);
  if (c == 0x9C) {
    //send_data_(); //send data after delimiter
    std::vector<uint8_t> frame(this->rx_message_.begin(), this->rx_message_.end());
    //after keepalive from heater 98.40.06.6D.3A.9C better time to send data?
    //if (frame[4] == 0x3A) this->send_data_();
    // for debug
    if ((frame[4] != 0xE3) && (frame[4] != 0x3A )) {
      ESP_LOGCONFIG(TAG, "Previous frame %zuus Received in %zuus %s" , micros()-this->last_frame_, micros()-now_micros,format_hex_pretty(frame).c_str());
    }
    this->last_frame_ = now_micros; //for debug
    this->handle_frame_(frame);
    this->rx_message_.clear();
    return;
  }
  this->rx_message_.push_back(c);
}

 void SAUNA360Component::send_data_() {

    // Send the next packet in the queue
    if (!this->tx_queue_.empty()) {
      auto packet = std::move(this->tx_queue_.front());
      this->tx_queue_.pop();
      ESP_LOGCONFIG(TAG, "%zu SENDING FROM TX QUEUE: %s" , millis(), format_hex_pretty(packet).c_str()); // for debug
      
      // signal flow control write mode enabled
      if (this->flow_control_pin_ != nullptr) {
        this->flow_control_pin_->digital_write(true);
        delayMicroseconds(20); // delay before sending
        ESP_LOGCONFIG(TAG, "FLOW CONTROL ON"); // for debug
      }
      this->write_byte(0x00);
      this->write_array(packet);
      this->flush();
      ESP_LOGCONFIG(TAG, "DATA SENT SUCCESFULLY"); // for debug

      // signal flow control write mode disabled
      if (this->flow_control_pin_ != nullptr) {
        delayMicroseconds(2000); // delay after sending
        this->flow_control_pin_->digital_write(false);
        ESP_LOGCONFIG(TAG, "FLOW CONTROL OFF"); // for debug
      }
    }
  }

void SAUNA360Component::handle_frame_(std::vector<uint8_t> frame) {
  // Decode the frame
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
        // The EOF byte
        d = 0x9c;
      }
      else if (d == 0x67){
        // The SOF byte
        d = 0x98;
      }
      else if (d == 0x6E){
        // The ESC byte itself
        d = 0x91;
      }
      else{
        ESP_LOGCONFIG(TAG, "Unknown escape sequence: %02x" ,d);
      packet.push_back(0x91);
      }
    }
    packet.push_back(d);
  }

   if (crc16be(packet.data(), 16, 0xffff, 0x90d9, false, false)){
     packet.pop_back();
     this->handle_packet_(packet);
   }
   else {
    ESP_LOGCONFIG(TAG, "CRC ERROR");
   }
   frame.clear();
}

void SAUNA360Component::handle_packet_(std::vector<uint8_t> packet) {
  // Decode the packet, but skip keepalive handshakes
  size_t len = packet.size();
  if (len <= 2){
    packet.clear();
    return;
  }
  uint8_t address = packet[0];
  uint8_t packetType = packet[1];

  uint16_t code = ((uint16_t) packet[2]) << 8; // add MSB 
  code |= ((uint16_t) packet[3]); // add LSB

  uint32_t data = ((uint32_t) packet[4]) << 24; // add MSB
  data |= ((uint32_t) packet[5]) << 16; // next byte
  data |= ((uint32_t) packet[6]) << 8; // next byte
  data |= ((uint32_t) packet[7]); // LSB"

  //Only take codes from the heater to control
  //if ((packetType == 0x07) || (packetType == 0x09)) {
    //packet.clear();
    //return;
  //}

  if (code == 0x6000){
    // temperature data point. Split into set point and actual value
    int actualTemp = (data & 0x00007FF) / 9.0;
    for (auto &listener : listeners_) {listener->on_temperature(actualTemp);}
    int setPointTemp = ((data >> 11) & 0x00007FF) / 9.0;
    for (auto &listener : listeners_) {listener->on_temperature_setting(setPointTemp);}
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
    ESP_LOGCONFIG(TAG, "Packet: %s" ,format_hex_pretty(packet).c_str());
    ESP_LOGCONFIG(TAG, "Code: %04x" ,code);
    ESP_LOGCONFIG(TAG, "Data: %08x" ,data);
  }

  packet.clear();
}

void SAUNA360Component::apply_heater_on_action() {
  std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x40, 0x95, 0x59, 0x9C });
  this->tx_queue_.push(send_packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER ON");
    return;
  }

void SAUNA360Component::apply_heater_off_action() {
  std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x80, 0x8D, 0x1E, 0x9C });
  this->tx_queue_.push(send_packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER OFF");
    return;
  }

void SAUNA360Component::apply_heater_standby_action() {
  std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x0A, 0x94, 0x9C });
  this->tx_queue_.push(send_packet);
  ESP_LOGCONFIG(TAG, "SETTING HEATER STANDBY");
    return;
  }

void SAUNA360Component::apply_heater_power_toggle_action() {
  std::vector<uint8_t> send_packet({ 0x98, 0x40, 0x07, 0x70, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0x0A, 0x9C});
  this->tx_queue_.push(send_packet);
  ESP_LOGCONFIG(TAG, "POWER TOGGLE");
    return;
  }

void SAUNA360Component::set_bath_time(uint8_t value) {

}

void SAUNA360Component::set_bath_time_number(uint8_t value) {

}

void SAUNA360Component::set_bath_temperature(uint8_t value) {

}

void SAUNA360Component::set_bath_temperature_number(uint8_t value) {

}

void SAUNA360Component::dump_config(){
    ESP_LOGCONFIG(TAG, "UART component");
    LOG_PIN("Flow Control Pin:", this->flow_control_pin_);
    ESP_LOGCONFIG(TAG, "Default Bath Time: %f", bath_time_default_);
}

}  // namespace sauna360
}  // namespace esphome