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
#include <expected>
#include <memory>

#include <gpio/PinLevel.hpp>
#include <gpio/interface/OutputPin.hpp>

/**
 * @namespace gpio
 */
namespace gpio {

/**
 * @class OutputPin
 * @brief
 */
class OutputPin : public interface::OutputPin<PinLevel> {
public:
  enum class Error : std::uint8_t {
    CONFIGURATION_PIN_FAILED,
  };

public:
  using Pin = std::uint8_t;
  using Pointer = std::unique_ptr<OutputPin>;

public:
  static auto create(Pin numberOfPin) noexcept -> std::expected<Pointer, Error>;
  static auto create(Pin numberOfPin, PinLevel defaultLevel) noexcept -> std::expected<Pointer, Error>;

private:
  explicit OutputPin(Pin numberOfPin) noexcept;

public:
  ~OutputPin() noexcept override = default;

public:
  [[nodiscard]] [[maybe_unused]] auto getLevel() const -> PinLevel override;

public:
  auto setLevel(PinLevel value) const -> void override;

private:
  Pin const m_numberOfPin;
};

} // namespace gpio
