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

#include "gpio/OutputPin.hpp"

#include <esp_err.h>
#include <driver/gpio.h>

#include "gpio/PinLevel.hpp"

namespace gpio {

namespace {

auto const PIN_LEVEL_DEFAULT = PinLevel::LOW;

using Value = std::uint8_t;
using Result = esp_err_t;

} // namespace

auto OutputPin::create(OutputPin::Pin const numberOfPin) noexcept -> std::expected<OutputPin::Pointer, OutputPin::Error> { return OutputPin::create(numberOfPin, PIN_LEVEL_DEFAULT); }

auto OutputPin::create(OutputPin::Pin const numberOfPin, PinLevel const defaultLevel) noexcept -> std::expected<OutputPin::Pointer, OutputPin::Error> {
  gpio_config_t gpioConfig = {
      .pin_bit_mask = (1ull << numberOfPin),
      .mode = GPIO_MODE_INPUT_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_ENABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };

  if (defaultLevel == PinLevel::HIGH) {
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
  }

  Result const configurationResult = gpio_config(&gpioConfig);
  if (configurationResult != ESP_OK) {
    return std::unexpected(OutputPin::Error::CONFIGURATION_PIN_FAILED);
  }

  auto instance = OutputPin::Pointer(new OutputPin(numberOfPin));

  instance->setLevel(defaultLevel);

  return std::move(instance);
}

OutputPin::OutputPin(Pin const numberOfPin) noexcept : m_numberOfPin(numberOfPin) {
  static_assert(static_cast<Value>(PinLevel::LOW) == 0);
  static_assert(static_cast<Value>(PinLevel::HIGH) == 1);
}

auto OutputPin::setLevel(PinLevel value) const -> void { gpio_set_level(static_cast<gpio_num_t>(m_numberOfPin), static_cast<Value>(value)); }

auto OutputPin::getLevel() const -> PinLevel {
  auto const gpioLevel = gpio_get_level(static_cast<gpio_num_t>(m_numberOfPin));
  auto const level = static_cast<PinLevel>(gpioLevel);

  return level;
}

} // namespace gpio
