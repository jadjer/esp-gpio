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

#pragma once

#include <cstdint>

#include "gpio/PinLevel.hpp"

/**
 * @namespace gpio
 */
namespace gpio {

/**
 * @class Pin
 * @brief
 */
class InputPin {
public:
  explicit InputPin(std::uint8_t numberOfPin, PinLevel defaultLevel = PIN_LEVEL_LOW);
  ~InputPin() = default;

public:
  void countReset();

public:
  [[nodiscard]] PinLevel getLevel() const;
  [[nodiscard]] std::uint32_t getCount() const;
  [[nodiscard]] std::uint32_t getDelay() const;

private:
  void process();

private:
  PinLevel const m_defaultLevel;
  std::uint8_t const m_numberOfPin;

private:
  PinLevel m_level;

private:
  std::uint32_t m_count;
  std::uint32_t m_lastUpdate_InMicroseconds;

private:
  static void isr(void *arg);
};

}// namespace gpio

#include <memory>

using InputPinPtr = std::unique_ptr<gpio::InputPin>;
