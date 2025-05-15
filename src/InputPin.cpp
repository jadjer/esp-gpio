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
#include <esp_err.h>
#include <esp_timer.h>

#include "gpio/PinLevel.hpp"

namespace gpio {

namespace {

auto const PIN_LEVEL_DEFAULT = PinLevel::LOW;
auto const MINIMAL_PULSE_DELAY_MICROSECONDS = 10000; // 0.01 sec

using Value = std::uint8_t;
using Result = esp_err_t;

} // namespace

auto InputPin::create(InputPin::Pin const numberOfPin) noexcept -> std::expected<InputPin::Pointer, InputPin::Error> { return InputPin::create(numberOfPin, PIN_LEVEL_DEFAULT); }

auto InputPin::create(InputPin::Pin numberOfPin, PinLevel defaultLevel) noexcept -> std::expected<InputPin::Pointer, InputPin::Error> {
  gpio_config_t gpioConfig = {
      .pin_bit_mask = (1ull << numberOfPin),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_ENABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };

  if (defaultLevel == PinLevel::HIGH) {
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  }

  Result const configurationResult = gpio_config(&gpioConfig);
  if (configurationResult != ESP_OK) {
    return std::unexpected(InputPin::Error::CONFIGURATION_PIN_FAILED);
  }

  auto instance = InputPin::Pointer(new (std::nothrow) InputPin(numberOfPin, defaultLevel));

  Result const installServiceResult = gpio_install_isr_service(0);
  if ((installServiceResult != ESP_OK) and (installServiceResult != ESP_ERR_INVALID_STATE)) {
    return std::unexpected(InputPin::Error::CONFIGURATION_ISR_FAILED);
  }

  Result const addHandleResult = gpio_isr_handler_add(static_cast<gpio_num_t>(numberOfPin), InputPin::isr, instance.get());
  if (addHandleResult != ESP_OK) {
    return std::unexpected(InputPin::Error::CONFIGURATION_ISR_FAILED);
  }

  return std::move(instance);
}

auto InputPin::isr(void *arg) -> void {
  auto *self = static_cast<InputPin *>(arg);
  self->process();
}

InputPin::InputPin(InputPin::Pin const numberOfPin, PinLevel const defaultLevel) noexcept
    : m_numberOfPin(numberOfPin), m_defaultLevel(defaultLevel), m_count(0), m_level(PinLevel::UNKNOWN), m_lastUpdate(0), m_minimalPulseDelay(MINIMAL_PULSE_DELAY_MICROSECONDS) {

  static_assert(static_cast<Value>(PinLevel::LOW) == 0);
  static_assert(static_cast<Value>(PinLevel::HIGH) == 1);

  process();
}

auto InputPin::getLevel() const -> PinLevel { return m_level; }

auto InputPin::getCount() const -> Count { return m_count; }

auto InputPin::getDelay() const -> Delay {
  auto const currentTime = esp_timer_get_time();
  auto const timeDifference = currentTime - m_lastUpdate;

  return timeDifference;
}

[[maybe_unused]] auto InputPin::setMinimalPulseDelay(InputPin::Time const minimalPulseDelay) -> void { m_minimalPulseDelay = minimalPulseDelay; }

[[maybe_unused]] void InputPin::countReset() { m_count = 0; }

auto InputPin::process() -> void {
  if (isNoise()) {
    return;
  }

  auto const gpioLevel = gpio_get_level(static_cast<gpio_num_t>(m_numberOfPin));

  m_level = static_cast<PinLevel>(gpioLevel);
  if (m_level == m_defaultLevel) {
    return;
  }

  m_count += 1;
}

auto InputPin::isNoise() -> bool {
  auto const currentTime = esp_timer_get_time();
  auto const timeDifference = currentTime - m_lastUpdate;

  m_lastUpdate = currentTime;

  if (timeDifference <= m_minimalPulseDelay) {
    return true;
  }

  return false;
}

} // namespace gpio
