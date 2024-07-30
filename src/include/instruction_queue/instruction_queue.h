#pragma once

#include <cstdint>

#include "alu/imm_gen.h"
#include "alu/reservation_station.h"
#include "decoder/instruction_decoder.h"
#include "instruction_queue/branch_predictor.h"
#include "memory/load_store_buffer.h"
#include "reg_file/register_status.h"
#include "reorder_buffer/reorder_buffer.h"

struct InstructionQueueEntry {
  ALUType alu_type{};
  ImmType imm_type{};
  OpType op_type{};
  BrType branch_type{};
  uint32_t instruction{};
  uint32_t immediate{};
  uint32_t address{};
  bool branch_prediction{false};
};

class InstructionQueue {
public:
  InstructionQueue() = delete;
  InstructionQueue(LoadStoreBuffer &lsb, ReservationStation &rs, RegisterStatus &register_status, ReorderBuffer &rob);
  void Tick();

private:
  uint32_t program_counter_{0};
  SaturatingPredictor predictor_{};
  InstructionDecoder decoder_{};
  ImmGen imm_gen_{};
  bool stall_{false};
  bool stall_load_{false};
  int head_{0};
  int tail_{0};
  InstructionQueueEntry instructions_[32]{};

  LoadStoreBuffer &lsb_;
  ReservationStation &rs_;
  RegisterStatus &register_status_;
  ReorderBuffer &reorder_buffer_;
};