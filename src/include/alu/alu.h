#pragma once

#include "decoder/instruction_decoder.h"

class ALU {
public:
  ALU() = default;
  uint32_t Calculate(uint32_t a, uint32_t b, ALUType type);
  void Tick();

private:
  // Debugging variables
  bool calc_{};
};
