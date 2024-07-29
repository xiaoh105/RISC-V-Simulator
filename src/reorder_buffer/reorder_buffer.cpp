#include <cassert>
#include <iostream>

#include "reorder_buffer/reorder_buffer.h"
#include "wiring.h"

union Word {
  int32_t sign;
  uint32_t unsign;
};

void ReorderBuffer::Tick() {
  wire_in.rob_branch_result_enable = false;
  wire_in.regfile_reset = wire_in.rob_reset = wire_in.rs_reset = wire_in.lsb_reset = false;
  wire_in.lsb_store_ready = false;
  wire_in.regfile_write_enable = false;
  if (wire_out.rob_reset) {
    head_ = tail_ = 0;
    return;
  }
  if (wire_out.writeback1_enable) {
    for (auto &entry : entries_) {
      if (!entry.operand1_ready && entry.operand1_id == wire_out.writeback1_id) {
        entry.operand1_val = wire_out.writeback1_val;
        entry.operand1_ready = true;
      }
      if (!entry.operand2_ready && entry.operand2_id == wire_out.writeback1_id) {
        entry.operand2_val += wire_out.writeback1_val;
        entry.operand2_ready = true;
      }
    }
  }
  if (wire_out.writeback2_enable) {
    for (auto &entry : entries_) {
      if (!entry.operand1_ready && entry.operand1_id == wire_out.writeback2_id) {
        entry.operand1_val = wire_out.writeback2_val;
        entry.operand1_ready = true;
      }
      if (!entry.operand2_ready && entry.operand2_id == wire_out.writeback2_id) {
        entry.operand2_val += wire_out.writeback2_val;
        entry.operand2_ready = true;
      }
    }
  }
  auto &entry = entries_[head_];
  if (entry.imm_type == ImmType::R
    || entry.imm_type == ImmType::I && entry.op_type != OpType::Branch
    || entry.imm_type == ImmType::IShift
    || entry.imm_type == ImmType::U
    || entry.imm_type == ImmType::J) {
    if (entry.operand1_ready) {
      wire_out.regfile_write_enable = true;
      wire_out.regfile_write_dependency = head_;
      wire_out.regfile_write_id = entry.operand1_id;
      wire_out.regfile_write_val = entry.operand1_val;
      head_ = (head_ + 1) % 32;
    }
  } else if (entry.imm_type == ImmType::I) {
    assert(entry.op_type == OpType::Branch);
    if (entry.operand1_ready && entry.operand2_ready) {
      wire_in.rob_set_pc_enable = true;
      wire_in.rob_set_pc = entry.operand2_val;
      wire_in.regfile_write_enable = true;
      wire_in.regfile_write_dependency = head_;
      wire_in.regfile_write_id = entry.operand1_id;
      wire_in.regfile_write_val = entry.operand1_val;
      head_ = (head_ + 1) % 32;
    }
  } else if (entry.imm_type == ImmType::S) {
    wire_in.lsb_store_ready = true;
    head_ = (head_ + 1) % 32;
  } else if (entry.imm_type == ImmType::B) {
    if (entry.operand1_ready && entry.operand2_ready) {
      bool branch = false;
      switch (entry.branch_type) {
        case BrType::Eq:
          branch = entry.operand1_val == entry.operand2_val;
          break;
        case BrType::Ne:
          branch = entry.operand1_val != entry.operand2_val;
          break;
        case BrType::GeU:
          branch = entry.operand1_val >= entry.operand2_val;
          break;
        case BrType::LtU:
          branch = entry.operand1_val < entry.operand2_val;
          break;
        case BrType::Lt: {
          Word lhs{}, rhs{};
          lhs.unsign = entry.operand1_val;
          rhs.unsign = entry.operand2_val;
          branch = lhs.sign < rhs.sign;
          break;
        }
        case BrType::Ge: {
          Word lhs{}, rhs{};
          lhs.unsign = entry.operand1_val;
          rhs.unsign = entry.operand2_val;
          branch = lhs.sign >= rhs.sign;
          break;
        }
      }
      wire_in.rob_branch_result_enable = true;
      wire_in.rob_branch_result_instruction_address = entry.instruction_addr;
      wire_in.rob_branch_result_take_branch = branch;
      wire_in.rob_branch_result_predict_fail = branch != entry.branch_prediction;
      if (branch != entry.branch_prediction) {
        wire_in.rob_set_pc_enable = true;
        wire_in.rob_set_pc = branch ? entry.branch_addr : entry.instruction_addr + 4;
        wire_in.regfile_reset = true;
        wire_in.rob_reset = true;
        wire_in.rs_reset = true;
        wire_in.lsb_reset = true;
      }
      head_ = (head_ + 1) % 32;
    }
  } else {
    std::cerr << "Invalid immediate type when poping Reorder Buffer" << std::endl;
    throw std::runtime_error("Invalid Imm Type");
  }
  if (wire_out.rob_append_enable) {
    if ((tail_ + 1) % 32 == head_) {
      std::cerr << "Receving append signal when rob is full" << std::endl;
      throw std::runtime_error("Invalid append signal");
    }
    entry = entries_[tail_];
    entry.imm_type = wire_out.rob_append_imm_type;
    entry.op_type = wire_out.rob_append_op_type;
    entry.operand1_ready = wire_out.rob_append_operand1_ready;
    entry.operand1_id = wire_out.rob_append_operand1_id;
    entry.operand1_val = wire_out.rob_append_operand1_val;
    entry.operand2_ready = wire_out.rob_append_operand2_ready;
    entry.operand2_id = wire_out.rob_append_operand2_id;
    entry.operand2_val = wire_out.rob_append_operand2_val;
    entry.instruction_addr = wire_out.rob_append_instruction_address;
    entry.branch_addr = wire_out.rob_append_branch_address;
    entry.branch_type = wire_out.rob_append_branch_type;
    entry.branch_prediction = wire_out.rob_append_predict_branch;
    tail_ = (tail_ + 1) % 32;
  }
}

int ReorderBuffer::GetNextId() const {
  if (wire_out.rob_append_enable) {
    return (tail_ + 1) % 32;
  }
  return tail_;
}

ReplyReorderBuffer ReorderBuffer::Query(uint32_t reg_id) const {
  if (reg_id >= 32) {
    std::cerr << "Invalid register id in Reorder Buffer" << std::endl;
    throw std::runtime_error("Invalid register id");
  }
  if (head_ <= tail_ && (reg_id < head_ || reg_id >= tail_)) {
    std::cerr << "Visiting virtual registers outside the queue in RoB" << std::endl;
    throw std::runtime_error("Invalid virtual register id");
  }
  if (head_ > tail_ && (reg_id >= tail_ && reg_id < head_)) {
    std::cerr << "Visiting virtual registers outside the queue in RoB" << std::endl;
    throw std::runtime_error("Invalid virtual register id");
  }
  const auto &entry = entries_[reg_id];
  if (entry.imm_type == ImmType::B || entry.imm_type == ImmType::S) {
    std::cerr << "Requiring virtual register in B Type/S Type instructions" << std::endl;
    throw std::runtime_error("Invalid virtual register");
  }
  ReplyReorderBuffer ret{};
  ret.ready = entry.operand1_ready;
  ret.val = entry.operand1_val;
  return ret;
}

bool ReorderBuffer::IsFull() const {
  return (tail_ + 1) % 32 == head_ || (tail_ + 2) % 32 == head_;
}
