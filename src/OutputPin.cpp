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

#include "gpio/OutputPin.hpp"

#include <cassert>

#include <driver/gpio.h>

namespace gpio {

OutputPin::OutputPin(std::uint8_t const numberOfPin, PinLevel const defaultLevel) : m_numberOfPin(numberOfPin) {
  assert(PIN_LEVEL_LOW == 0);
  assert(PIN_LEVEL_HIGH == 1);

  gpio_config_t gpioConfig = {
      .pin_bit_mask = (1ull << m_numberOfPin),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE,
  };

  gpio_config(&gpioConfig);
  gpio_set_level(static_cast<gpio_num_t>(m_numberOfPin), defaultLevel);
}

void OutputPin::setLevel(PinLevel value) {
  gpio_set_level(static_cast<gpio_num_t>(m_numberOfPin), value);
}

}
