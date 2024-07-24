#include <climits>
#include <cstddef>
#include <iostream>
#include "memory/memory.h"

union HalfWord {
  int16_t sign;
  uint16_t unsign;
};

union Word {
  int32_t sign;
  uint32_t unsign;
};

Memory::Memory(int clock_per_visit, const std::unordered_map<uint32_t, std::byte>& memory)
: clock_per_visit_(clock_per_visit), memory_(memory) {}

void Memory::Tick() {
  if (busy_) {
    ++turn_;
    if (turn_ == clock_per_visit_) {
      turn_ = 0;
      busy_ = false;
    }
  }
}

uint32_t Memory::Load(uint32_t addr, Type type, bool signed_ext) {
  if (busy_ && addr == current_op_) {
    std::cerr << "Calling LOAD when the address is under modification." << std::endl;
    throw std::runtime_error("Invalid load operation");
  }
  Word data{};
  switch (type) {
    case Type::byte: {
      if (signed_ext) {
        data.sign = static_cast<char>(memory_[addr]); // NOLINT
      } else {
        data.unsign = static_cast<unsigned char>(memory_[addr]);
      }
      break;
    }
    case Type::halfword: {
      HalfWord val{};
      val.unsign = static_cast<uint16_t>(memory_[addr + 1]) << 8
        | static_cast<uint16_t>(memory_[addr]);
      if (signed_ext) {
        data.sign = val.sign;
      } else {
        data.unsign = val.unsign;
      }
      break;
    }
    case Type::word: {
      data.unsign = 0;
      for (int i = 3; i >= 0; --i) {
        data.unsign <<= 8;
        data.unsign |= static_cast<uint32_t>(memory_[addr + i]);
      }
      break;
    }
  }
  return data.unsign;
}

void Memory::Store(uint32_t addr, uint32_t data, Type type) {
  if (busy_) {
    std::cerr << "Calling store while the memory store is still busy." << std::endl;
    throw std::runtime_error("Invalid store operation");
  }
  busy_ = true;
  turn_ = 0;
  current_op_ = addr;
  switch (type) {
    case Type::byte : {
      memory_[addr] = static_cast<std::byte>(data & UCHAR_MAX);
      break;
    }
    case Type::halfword : {
      memory_[addr] = static_cast<std::byte>(data & UCHAR_MAX);
      memory_[addr + 1] = static_cast<std::byte>(data >> 8 & UCHAR_MAX);
      break;
    }
    case Type::word : {
      for (int i = 0; i < 4; ++i) {
        memory_[addr + i] = static_cast<std::byte>(data & UCHAR_MAX);
        data >>= 8;
      }
      break;
    }
  }
}
