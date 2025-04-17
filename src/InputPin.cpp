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

#include <driver/gpio.h>
#include <esp_timer.h>

#include "gpio/PinLevel.hpp"

namespace gpio {

auto const MINIMAL_PULSE_DELAY_MICROSECONDS = 10000; // 0.01 sec

InputPin::InputPin(InputPin::Pin const numberOfPin, PinLevel const defaultLevel)
    : m_numberOfPin(numberOfPin), m_defaultLevel(defaultLevel), m_level(PIN_LEVEL_UNKNOWN), m_count(0), m_lastUpdate(0) {

  static_assert(PIN_LEVEL_LOW == 0);
  static_assert(PIN_LEVEL_HIGH == 1);

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

[[maybe_unused]] void InputPin::countReset() { m_count = 0; }

auto InputPin::getLevel() const -> PinLevel { return m_level; }

auto InputPin::getCount() const -> Count { return m_count; }

auto InputPin::getDelay() const -> Delay {
  auto const currentTime_InMicroseconds = esp_timer_get_time();
  auto const delay_InMicroseconds = currentTime_InMicroseconds - m_lastUpdate;

  return delay_InMicroseconds;
}

auto InputPin::process() -> void {
  auto const currentTime_InMicroseconds = esp_timer_get_time();
  auto const diffTime_InMicroseconds = currentTime_InMicroseconds - m_lastUpdate;

  if (diffTime_InMicroseconds <= MINIMAL_PULSE_DELAY_MICROSECONDS) {
    return;
  }

  auto const gpioLevel = gpio_get_level(static_cast<gpio_num_t>(m_numberOfPin));

  m_level = static_cast<PinLevel>(gpioLevel);
  if (m_level == m_defaultLevel) {
    return;
  }

  m_count += 1;
  m_lastUpdate = currentTime_InMicroseconds;
}

auto InputPin::isr(void *arg) -> void {
  auto *const self = static_cast<InputPin *>(arg);

  self->process();
}

} // namespace gpio
