#include "magic_switch.h"
#include "esphome/core/log.h"

namespace esphome {
namespace magic_switch {

static const char *const TAG = "magic_switch";

void MagicSwitch::setup() {
  this->pin_->setup();
  this->isr_pin_ = this->pin_->to_isr();
  this->pin_->attach_interrupt(MagicSwitch::edge_intr, this, gpio::INTERRUPT_ANY_EDGE);
}

void MagicSwitch::loop() {
  uint32_t pulse = this->pulse_;
  this->pulse_ = 0;
  if (pulse) {
    ESP_LOGD(TAG, "detected pulse of %uus", pulse);
    this->switch_trigger_->trigger();
  }
}

float MagicSwitch::get_setup_priority() const { return setup_priority::DATA; }

void MagicSwitch::dump_config() {
  ESP_LOGCONFIG(TAG, "");
  LOG_PIN("  Pin: ", this->pin_);
  ESP_LOGCONFIG(TAG, "  Timeout: %uus", this->timeout_);
  if (this->ignore_pulse_max_ > 0) {
    ESP_LOGCONFIG(TAG, "  Ignore pulse range: %uus - %uus", this->ignore_pulse_min_, this->ignore_pulse_max_);
  }
}

void IRAM_ATTR HOT MagicSwitch::edge_intr(MagicSwitch *comp) {
  uint32_t now = micros();
  bool state = comp->isr_pin_.digital_read();
  if (state) {
    comp->pulse_start_ = now;
  } else if (comp->pulse_start_) {
    uint32_t diff = now - comp->pulse_start_;
    if (diff > comp->timeout_) {
      // Check if pulse should be ignored based on ignore range
      bool should_ignore = false;
      if (comp->ignore_pulse_max_ > 0) {
        should_ignore = (diff >= comp->ignore_pulse_min_ && diff <= comp->ignore_pulse_max_);
      }
      if (!should_ignore) {
        comp->pulse_ = diff;
      }
    }
  }
}

}  // namespace magic_switch
}  // namespace esphome
