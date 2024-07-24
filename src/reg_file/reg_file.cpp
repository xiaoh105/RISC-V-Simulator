#include <iostream>

#include "reg_file/reg_file.h"

uint32_t RegFile::Load(uint32_t addr) {
  if (addr >= REGNUM) {
    std::cerr << "Invalid register address " << addr << std::endl;
    throw std::runtime_error("Invalid register");
  }
  if (read_ == 2) {
    std::cerr << "Register read > 2 in one tick" << std::endl;
    throw std::runtime_error("Invalid read");
  }
  ++read_;
  return registers_[addr];
}

void RegFile::Store(uint32_t addr, uint32_t value) {
  if (addr > REGNUM) {
    std::cerr << "Invalid register address " << addr << std::endl;
    throw std::runtime_error("Invalid register");
  }
  if (write_ == 1) {
    std::cerr << "Register write > 1 in one tick" << std::endl;
    throw std::runtime_error("Invalid write");
  }
  ++write_;
  registers_[addr] = value; // NOLINT
}

void RegFile::Tick() {
  write_ = 0;
  read_ = 0;
}

