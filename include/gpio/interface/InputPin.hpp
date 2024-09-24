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
// Created by jadjer on 02.10.23.
//

#pragma once

#include <cstdint>

namespace gpio::interface {

template<class T>
class InputPin {
public:
  virtual ~InputPin() = default;

public:
  [[nodiscard]] virtual T getLevel() const = 0;
  [[nodiscard]] virtual uint64_t getCount() const = 0;
  [[nodiscard]] virtual uint64_t getDelay() const = 0;
};

}

#include <memory>

template<class T>
using InputPinPtr = std::unique_ptr<gpio::interface::InputPin<T>>;
