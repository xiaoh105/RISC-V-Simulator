#pragma once
#include <cstdint>

constexpr int REGNUM = 32;

class RegFile {
public:
  RegFile() = default;
  uint32_t Load(uint32_t addr);
  void Store(uint32_t addr, uint32_t value);
  void Tick();

private:
  uint32_t registers_[32]{};
  // Debugging variables
  int read_{};
  int write_{};
};
