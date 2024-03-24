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

namespace gpio::interface {

template<class T>
class IOutputPin {
public:
  virtual ~IOutputPin() = default;

public:
  virtual void setLevel(T value) = 0;
};

}

#include <memory>

template<class T>
using IOutputPinPtr = std::unique_ptr<gpio::interface::IOutputPin<T>>;
