// Copyright 2024 Pavel Suprunov
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

//
// Created by jadjer on 30.08.22.
//

#include "gpio/InputPin.hpp"

#include <cassert>

#include <driver/gpio.h>
#include <esp_timer.h>

namespace gpio {

InputPin::InputPin(uint8_t const numberOfPin, PinLevel const defaultLevel) : m_numberOfPin(numberOfPin),
                                                                             m_defaultLevel(defaultLevel),
                                                                             m_level(PIN_LEVEL_UNKNOWN),
                                                                             m_count(0),
                                                                             m_lastUpdate(0) {
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

PinLevel InputPin::getLevel() const {
  return m_level;
}

uint64_t InputPin::getCount() const {
  return m_count;
}

uint64_t InputPin::getDelay() const {
  auto const currentTime = esp_timer_get_time();
  auto const delay = currentTime - m_lastUpdate;

  return delay;
}

void InputPin::process() {
  auto const gpioLevel = gpio_get_level(static_cast<gpio_num_t>(m_numberOfPin));
  m_level = static_cast<PinLevel>(gpioLevel);

  if (m_level == m_defaultLevel) {
    return;
  }

  auto const currentTime = esp_timer_get_time();

  m_count += 1;
  m_lastUpdate = currentTime;
}

void InputPin::isr(void *arg) {
  auto const inputPin = static_cast<InputPin *>(arg);

  inputPin->process();
}

}// namespace gpio
