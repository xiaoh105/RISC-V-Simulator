#include <iostream>

#include "instruction_queue/instruction_queue.h"
#include "util.hpp"
#include "wiring.h"

InstructionQueue::InstructionQueue(LoadStoreBuffer& lsb, ReservationStation& rs, RegisterStatus& register_status, ReorderBuffer& rob)
  : lsb_(lsb), rs_(rs), register_status_(register_status), reorder_buffer_(rob) {}

void InstructionQueue::Tick() {
  wire_in.instruction_queue_drop = false;
  wire_in.lsb_instruction_load_enable = false;
  wire_in.lsb_append_enable = false;
  wire_in.rs_append_enable = false;
  wire_in.rob_append_enable = false;
  wire_in.regfile_append_enable = false;
  if (wire_out.rob_set_pc_enable) {
    stall_ = false;
    stall_load_ = false;
    program_counter_ = wire_out.rob_set_pc;
    head_ = tail_ = 0;
    wire_in.instruction_queue_drop = true;
  }
  if (stall_) {
    predictor_.Tick();
    decoder_.Tick();
    imm_gen_.Tick();
    return;
  }
  if (!wire_out.instruction_queue_drop && wire_out.lsb_instruction_return_enable && !stall_load_ && !wire_out.rob_set_pc_enable) {
    if ((tail_ + 1) % 32 == head_) {
      std::cerr << "Trying to append to Instruction Queue when it's already full" << std::endl;
      throw std::runtime_error("Invalid instruction fetch");
    }
    auto &op = instructions_[tail_];
    op.instruction = wire_out.lsb_instruction_return;
    op.address = wire_out.lsb_instruction_return_addr;
    decoder_.Decode(op.instruction);
    op.alu_type = decoder_.GetALUType();
    op.imm_type = decoder_.GetImmType();
    op.op_type = decoder_.GetOpType();
    op.branch_type = decoder_.GetBranchType();
    op.immediate = imm_gen_.FetchImmediate(op.instruction, op.imm_type, op.alu_type);
    if (op.imm_type == ImmType::J) {
      wire_in.instruction_queue_drop = true;
      program_counter_ = op.address + op.immediate;
    } else if (op.imm_type == ImmType::I && op.op_type == OpType::Branch) {
      wire_in.instruction_queue_drop = true;
      stall_load_ = true;
    } else if (op.imm_type == ImmType::B) {
      op.branch_prediction = predictor_.PredictBranch(op.address);
      if (op.branch_prediction) {
        program_counter_ = op.address + op.immediate;
        wire_in.instruction_queue_drop = true;
      }
    }
    tail_ = (tail_ + 1) % 32;
  }
  if ((tail_ + 1) % 32 != head_ && (tail_ + 2) % 32 != head_) {
    wire_in.lsb_instruction_load_enable = true;
    wire_in.lsb_instruction_load_addr = program_counter_;
    program_counter_ += 4;
  }
  if (head_ != tail_ && !reorder_buffer_.IsFull() && !lsb_.IsFull() && !rs_.IsFull()) {
    auto &op = instructions_[head_];
    auto reg1 = FetchBits(op.instruction, 15, 19);
    auto reg2 = FetchBits(op.instruction, 20, 24);
    auto res = register_status_.Query(reg1, reg2);
    auto dest = FetchBits(op.instruction, 7, 11);
    auto virtual_dest = reorder_buffer_.GetNextId();

    wire_in.rob_append_enable = true;
    wire_in.rob_append_imm_type = op.imm_type;
    wire_in.rob_append_op_type = op.op_type;
    wire_in.rob_append_predict_branch = op.branch_prediction;
    wire_in.rob_append_branch_address = op.address + op.immediate;
    wire_in.rob_append_instruction_address = op.address;
    wire_in.rob_append_branch_type = op.branch_type;

    wire_in.rob_append_operand1_ready = wire_in.rob_append_operand2_ready = false;
    wire_in.rob_append_rdest = dest;
    wire_in.rob_append_operand2_val = 0;

    wire_in.regfile_append_id = dest;
    wire_in.regfile_append_dependency = virtual_dest;
    wire_in.regfile_append_addr = op.address;

    wire_in.rs_append_dest = virtual_dest;
    wire_in.rs_append_alutype = op.alu_type;

    wire_in.lsb_append_width = static_cast<Memory::Type>(FetchBits(op.instruction, 12, 13));
    wire_in.lsb_append_sign_ext = FetchBits(op.instruction, 14, 14) == 0;

    if (!res.reg1_busy) {
      wire_in.rs_append_operand1_ready = true;
      wire_in.rob_append_operand1_ready = true;
      wire_in.lsb_append_base_ready = true;
      wire_in.rs_append_operand1 = res.reg1_val;
      wire_in.rob_append_operand1_val = res.reg1_val;
      wire_in.lsb_append_imm = res.reg1_val + op.immediate;
    } else {
      auto rename = reorder_buffer_.Query(res.reg1_dependency);
      if (rename.ready) {
        wire_in.rs_append_operand1_ready = true;
        wire_in.rob_append_operand1_ready = true;
        wire_in.lsb_append_base_ready = true;
        wire_in.rs_append_operand1 = rename.val;
        wire_in.rob_append_operand1_val = rename.val;
        wire_in.lsb_append_imm = rename.val + op.immediate;
      } else {
        wire_in.rs_append_operand1_ready = false;
        wire_in.rob_append_operand1_ready = false;
        wire_in.lsb_append_base_ready = false;
        wire_in.rs_append_operand1 = res.reg1_dependency;
        wire_in.rob_append_operand1_id = res.reg1_dependency;
        wire_in.lsb_append_base_reg = res.reg1_dependency;
        wire_in.lsb_append_imm = op.immediate;
      }
    }

    if (!res.reg2_busy) {
      wire_in.rs_append_operand2_ready = true;
      wire_in.rob_append_operand2_ready = true;
      wire_in.lsb_append_val_ready = true;
      wire_in.rs_append_operand2 = res.reg2_val;
      wire_in.rob_append_operand2_val = res.reg2_val;
      wire_in.lsb_append_val = res.reg2_val;
    } else {
      auto rename = reorder_buffer_.Query(res.reg2_dependency);
      if (rename.ready) {
        wire_in.rs_append_operand2_ready = true;
        wire_in.rob_append_operand2_ready = true;
        wire_in.lsb_append_val_ready = true;
        wire_in.rs_append_operand2 = rename.val;
        wire_in.rob_append_operand2_val = rename.val;
        wire_in.lsb_append_val = rename.val;
      } else {
        wire_in.rs_append_operand2_ready = false;
        wire_in.rob_append_operand2_ready = false;
        wire_in.lsb_append_val_ready = false;
        wire_in.rs_append_operand2 = res.reg2_dependency;
        wire_in.rob_append_operand2_id = res.reg2_dependency;
        wire_in.lsb_append_val = res.reg2_dependency;
      }
    }

    if (op.imm_type == ImmType::R) {
      wire_in.rs_append_enable = true;
      wire_in.regfile_append_enable = true;
      wire_in.rob_append_operand1_ready = false;
      wire_in.rob_append_operand1_id = virtual_dest;
    } else if (op.imm_type == ImmType::I || op.imm_type == ImmType::IShift) {
      if (op.op_type == OpType::Arith) {
        wire_in.rs_append_enable = true;
        wire_in.rs_append_operand2_ready = true;
        wire_in.rs_append_operand2 = op.immediate;
        wire_in.regfile_append_enable = true;
        wire_in.rob_append_operand1_ready = false;
        wire_in.rob_append_operand1_id = virtual_dest;
      } else if (op.op_type == OpType::MemRead) {
        wire_in.lsb_append_enable = true;
        wire_in.lsb_append_val = virtual_dest;
        wire_in.lsb_append_val_ready = true;
        wire_in.lsb_append_ready = true;
        wire_in.lsb_append_is_load = true;
        wire_in.regfile_append_enable = true;
        wire_in.rob_append_operand1_ready = false;
        wire_in.rob_append_operand1_id = virtual_dest;
      } else if (op.op_type == OpType::Branch) {
        stall_ = true;
        wire_in.regfile_append_enable = true;
        wire_in.rob_append_operand2_ready = wire_in.rob_append_operand1_ready;
        wire_in.rob_append_operand2_id = wire_in.rob_append_operand1_id;
        if (wire_in.rob_append_operand2_ready) {
          wire_in.rob_append_operand2_val = wire_in.rob_append_operand1_val + op.immediate;
        } else {
          wire_in.rob_append_operand2_val = op.immediate;
        }
        wire_in.rob_append_operand1_ready = true;
        wire_in.rob_append_operand1_val = op.address + 4;
      } else {
        std::cerr << "Invalid OpType in I Type instruction when decoding instruction" << std::endl;
        throw std::runtime_error("Invalid OpType");
      }
    } else if (op.imm_type == ImmType::S) {
      wire_in.lsb_append_enable = true;
      wire_in.lsb_append_ready = false;
      wire_in.lsb_append_is_load = false;
    } else if (op.imm_type == ImmType::U) {
      wire_in.rs_append_enable = true;
      wire_in.rs_append_operand2_ready = true;
      wire_in.rs_append_operand2 = op.immediate;
      wire_in.rs_append_operand1_ready = true;
      wire_in.rs_append_operand1 = op.address;
      wire_in.rob_append_operand1_ready = false;
      wire_in.rob_append_operand1_id = virtual_dest;
      wire_in.regfile_append_enable = true;
    } else if (op.imm_type == ImmType::B) {
      // Nothing to do
    } else if (op.imm_type == ImmType::J) {
      wire_in.regfile_append_enable = true;
      wire_in.rob_append_operand1_ready = true;
      wire_in.rob_append_operand1_val = op.address + 4;
    } else {
      std::cerr << "Invalid Immediate type when docoding instruction" << std::endl;
      throw std::runtime_error("Invalid ImmType");
    }
    if (op.instruction == 0x0ff00513) {
      stall_ = true;
    }
    head_ = (head_ + 1) % 32;
  }
  predictor_.Tick();
  decoder_.Tick();
  imm_gen_.Tick();
}
