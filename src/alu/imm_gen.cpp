#include <iostream>

#include "alu/imm_gen.h"
#include "util.hpp"

void ImmGen::Tick() {
  imm_ = false;
}

uint32_t ImmGen::FetchImmediate(uint32_t instruction, ImmType imm_type, ALUType alu_type) {
  if (imm_) {
    std::cerr << "Repeated immediate fetch" << std::endl;
    throw std::runtime_error("Invalid fetch immediate operation");
  }
  imm_ = true;
  if (imm_type == ImmType::R) {
    std::cerr << "Warn: Trying to get immediate from R Type instrucions" << std::endl;
    return 0;
  }
  if (imm_type == ImmType::I) {
    if (alu_type == ALUType::SetLTUnsigned) {
      return FetchBits(instruction, 20, 31);
    }
    return FetchBitsSigned(instruction, 20, 31);
  }
  if (imm_type == ImmType::IShift) {
    return FetchBits(instruction, 20, 24);
  }
  if (imm_type == ImmType::S) {
    auto lo = FetchBits(instruction, 7, 11);
    auto hi = FetchBits(instruction, 25, 31);
    auto val = hi << 5 | lo;
    val = SignExtend(val, 12);
    return val;
  }
  if (imm_type == ImmType::B) {
    auto bit12 = FetchBits(instruction, 31, 31);
    auto bit5_10 = FetchBits(instruction, 25, 30);
    auto bit1_4 = FetchBits(instruction, 8, 11);
    auto bit_11 = FetchBits(instruction, 7, 7);
    auto val = bit12 << 12 | bit5_10 << 5 | bit_11 << 11 | bit1_4 << 1;
    val = SignExtend(val, 13);
    return val;
  }
  if (imm_type == ImmType::U) {
    auto val = FetchBits(instruction, 12, 31);
    return val << 12;
  }
  if (imm_type == ImmType::J) {
    auto bit20 = FetchBits(instruction, 31, 31);
    auto bit1_10 = FetchBits(instruction, 21, 30);
    auto bit11 = FetchBits(instruction, 20, 20);
    auto bit12_19 = FetchBits(instruction, 12, 19);
    auto val = bit20 << 20 | bit1_10 << 1 | bit11 << 11 | bit12_19 << 12;
    val = SignExtend(val, 21);
    return val;
  }
  std::cerr << "Invalid immediate type when fetching immediate" << std::endl;
  throw std::runtime_error("Invalid ImmType");
}
