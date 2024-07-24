#pragma once
#include <cstdint>

#include "decoder/instruction_decoder.h"

class ImmGen {
public:
  ImmGen() = default;
  [[nodiscard]] uint32_t FetchImmediate(uint32_t instruction, ImmType imm_type, ALUType alu_type);
  void Tick();

private:
  // Debugging variables
  bool imm_;
};
