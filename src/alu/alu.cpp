#include <iostream>

#include "alu/alu.h"
#include "util.hpp"

union Word {
  int32_t sign;
  uint32_t unsign;
};

void ALU::Tick() {
  calc_ = false;
}

uint32_t ALU::Calculate(uint32_t a, uint32_t b, ALUType type) {
  if (calc_) {
    std::cerr << "Repeated ALU calculate within a single clock cycle" << std::endl;
    throw std::runtime_error("Invalid ALU operation");
  }
  calc_ = false;
  switch (type) {
    case ALUType::Add:
      return a + b;
    case ALUType::And:
      return a & b;
    case ALUType::Or:
      return a | b;
    case ALUType::Sub:
      return a - b;
    case ALUType::Xor:
      return a ^ b;
    case ALUType::ShiftL: {
      auto val = b & 0b11111;
      return a << val;
    }
    case ALUType::ShiftRArith: {
      auto val = b & 0b11111;
      a >>= val;
      return SignExtend(a, static_cast<int>(32 - val));
    }
    case ALUType::ShiftRLogical: {
      auto val = b & 0b11111;
      a >>= val;
      return a;
    }
    case ALUType::UseLatter:
      return b;
    case ALUType::SetLTSigned: {
      Word x{}, y{};
      x.unsign = a;
      y.unsign = b;
      return x.sign < y.sign;
    }
    case ALUType::SetLTUnsigned:
      return a < b;
    default: {
      std::cerr << "Invalid ALU Type when processing data" << std::endl;
      throw std::runtime_error("Invalid ALU Type");
    }
  }
}

