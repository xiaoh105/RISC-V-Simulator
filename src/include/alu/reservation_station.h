#pragma once

#include "alu/alu.h"

struct Entry {
  bool busy{false};
  uint32_t dest{};
  bool operand1_ready{false};
  bool operand2_ready{false};
  uint32_t operand1_val{false};
  uint32_t operand2_val{false};
  ALUType alu_type{false};
};

class ReservationStation {
public:
  ReservationStation() = default;
  void Tick();
  [[nodiscard]] bool IsFull() const;

private:
  ALU alu_;
  Entry entries_[32]{};
  int size_{0};
};
