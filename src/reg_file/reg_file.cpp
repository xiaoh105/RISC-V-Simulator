#include <iostream>

#include "reg_file/reg_file.h"

uint32_t RegFile::Load(uint32_t addr) {
  if (addr >= REGNUM) {
    std::cerr << "Invalid register address " << addr << std::endl;
    throw std::runtime_error("Invalid register");
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
  if (addr != 0) {
    registers_[addr] = value; // NOLINT
  }
}

void RegFile::Tick() {
  write_ = 0;
  read_ = 0;
}

void RegFile::Report() const {
  std::cout << "Reporting registers" << std::endl;
  for (const auto &val : registers_) {
    std::cout << val << " ";
  }
  std::cout << std::endl;
}
