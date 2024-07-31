#pragma once
#include <cstdint>
#include <iomanip>

#include "decoder/instruction_decoder.h"

inline int total_branch = 0;
inline int predicted_branch = 0;

inline void ReportBranch() {
  printf("Total branch: %d, predicted branch: %d. Prediction accuracy: %.2f%%\n",
    total_branch, predicted_branch, 100.0 * predicted_branch / total_branch);
}

struct RoBEntry {
  /// Metadata for the type of an instruction
  ImmType imm_type{};
  OpType op_type{};

  /// The destination register for regfile writeback
  uint32_t rdest{};

  /// Operand 1 saves:
  /// (1) Result for arithmetic/LOAD operations
  /// (2) PC + 4 for JALR and JAL
  /// (3) Lhs value for branch operations
  /// Notice: STORE operation requires nothing (managed by Load Store Buffer)
  bool operand1_ready{false};
  uint32_t operand1_id{};
  uint32_t operand1_val{};

  /// Operand2 saves:
  /// (1) New PC address for JALR
  /// (2) Rhs value for branch operations
  bool operand2_ready{false};
  uint32_t operand2_id{};
  uint32_t operand2_val{};

  /// Values below saves info required for branch and branch prediction
  uint32_t instruction_addr{};
  uint32_t branch_addr{};
  BrType branch_type{};
  bool branch_prediction{};

  bool terminate{false};
};

struct ReplyReorderBuffer {
  bool ready{};
  uint32_t val{};
};

class ReorderBuffer {
public:
  ReorderBuffer() = default;
  void Tick();
  [[nodiscard]] bool IsFull() const;
  [[nodiscard]] int GetNextId() const;
  void SetNextId();
  [[nodiscard]] ReplyReorderBuffer Query(uint32_t reg_id) const;

private:
  RoBEntry entries_[32]{};
  int head_{0};
  int tail_{0};
  int next_id_{};
};
