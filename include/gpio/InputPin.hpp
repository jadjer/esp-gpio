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
#include <expected>
#include <memory>

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
  enum class Error : std::uint8_t {
    CONFIGURATION_PIN_FAILED,
    CONFIGURATION_ISR_FAILED,
  };

public:
  using Pin = std::uint8_t;
  using Pointer = std::unique_ptr<InputPin>;

private:
  using Time = std::int64_t;

public:
  static auto create(Pin numberOfPin) noexcept -> std::expected<Pointer, Error>;
  static auto create(Pin numberOfPin, PinLevel defaultLevel) noexcept -> std::expected<Pointer, Error>;

private:
  static auto isr(void *arg) -> void;

private:
  InputPin(Pin numberOfPin, PinLevel defaultLevel) noexcept;

public:
  ~InputPin() noexcept override = default;

public:
  [[nodiscard]] auto getLevel() const -> PinLevel override;
  [[nodiscard]] [[maybe_unused]] auto getCount() const -> Count override;
  [[nodiscard]] [[maybe_unused]] auto getDelay() const -> Delay override;

public:
  [[maybe_unused]] auto setMinimalPulseDelay(Time minimalPulseDelay) -> void;

public:
  [[maybe_unused]] auto countReset() -> void;

private:
  auto process() -> void;
  auto isNoise() -> bool;

private:
  Pin const m_numberOfPin;
  PinLevel const m_defaultLevel;

private:
  Count m_count;
  PinLevel m_level;
  Time m_lastUpdate;
  Time m_minimalPulseDelay;
};

} // namespace gpio
