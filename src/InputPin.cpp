// Copyright 2025 Pavel Suprunov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "gpio/InputPin.hpp"

#include <cassert>

#include <driver/gpio.h>
#include <esp_timer.h>

namespace gpio {

auto const MINIMAL_PULSE_DELAY_MICROSECONDS = 10000;// 0.01 sec

InputPin::InputPin(std::uint8_t const numberOfPin, PinLevel const defaultLevel) : m_defaultLevel(defaultLevel),
                                                                                  m_numberOfPin(numberOfPin),
                                                                                  m_level(PIN_LEVEL_UNKNOWN),
                                                                                  m_count(0),
                                                                                  m_lastUpdate_InMicroseconds(0) {

  assert(PIN_LEVEL_LOW == 0);
  assert(PIN_LEVEL_HIGH == 1);

  gpio_config_t gpioConfig = {
      .pin_bit_mask = (1ull << m_numberOfPin),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_ENABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };

  if (m_defaultLevel == PIN_LEVEL_HIGH) {
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  }

  gpio_config(&gpioConfig);
  gpio_install_isr_service(0);
  gpio_isr_handler_add(static_cast<gpio_num_t>(m_numberOfPin), InputPin::isr, this);

  process();
}

void InputPin::countReset() {
  m_count = 0;
}

PinLevel InputPin::getLevel() const {
  return m_level;
}

std::uint32_t InputPin::getCount() const {
  return m_count;
}

std::uint32_t InputPin::getDelay() const {
  auto const currentTime_InMicroseconds = esp_timer_get_time();
  auto const delay_InMicroseconds = currentTime_InMicroseconds - m_lastUpdate_InMicroseconds;

  return delay_InMicroseconds;
}

void InputPin::process() {
  auto const currentTime_InMicroseconds = esp_timer_get_time();
  auto const diffTime_InMicroseconds = currentTime_InMicroseconds - m_lastUpdate_InMicroseconds;

  if (diffTime_InMicroseconds <= MINIMAL_PULSE_DELAY_MICROSECONDS) {
    return;
  }

  auto const gpioLevel = gpio_get_level(static_cast<gpio_num_t>(m_numberOfPin));

  m_level = static_cast<PinLevel>(gpioLevel);
  if (m_level == m_defaultLevel) {
    return;
  }

  m_count += 1;
  m_lastUpdate_InMicroseconds = currentTime_InMicroseconds;
}

void InputPin::isr(void *arg) {
  auto const inputPin = static_cast<InputPin *>(arg);

  inputPin->process();
}

}// namespace gpio
