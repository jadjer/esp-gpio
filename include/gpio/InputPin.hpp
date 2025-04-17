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

#pragma once

#include <cstdint>

#include <gpio/PinLevel.hpp>
#include <gpio/interface/InputPin.hpp>

/**
 * @namespace gpio
 */
namespace gpio {

/**
 * @class InputPin
 * @brief
 */
class InputPin : public interface::InputPin<PinLevel> {
public:
  using Pin = std::uint8_t;
  using Time = std::int64_t;

public:
  explicit InputPin(Pin numberOfPin, PinLevel defaultLevel = PIN_LEVEL_LOW);
  ~InputPin() override = default;

public:
  [[nodiscard]] auto getLevel() const -> PinLevel override;
  [[nodiscard]] [[maybe_unused]] auto getCount() const -> Count override;
  [[nodiscard]] [[maybe_unused]] auto getDelay() const -> Delay override;

public:
  [[maybe_unused]] auto countReset() -> void;

private:
  auto process() -> void;

private:
  Pin const m_numberOfPin;
  PinLevel const m_defaultLevel;

private:
  PinLevel m_level;

private:
  Count m_count;
  Time m_lastUpdate;

private:
  static void isr(void *arg);
};

} // namespace gpio
