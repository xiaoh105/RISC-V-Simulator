#pragma once
#include "alu/alu.h"
#include "alu/imm_gen.h"
#include "decoder/instruction_decoder.h"
#include "memory/decoder.h"
#include "memory/memory.h"
#include "reg_file/reg_file.h"

class BruteCPU {
public:
  BruteCPU() = delete;
  BruteCPU(const std::unordered_map<uint32_t, std::byte> &mem);
  void Tick();
  void Execute();

private:
  ALU alu_{};
  ImmGen imm_gen_{};
  InstructionDecoder decoder_{};
  Memory memory_;
  RegFile reg_file_{};
  uint32_t program_counter_{};
  // Debugging variables
  int clock_cnt_{};
};