#pragma once
#include <cstdint>

#include "decoder/instruction_decoder.h"
#include "memory/memory.h"


struct Wire {
  /// Flag for register dependency setting
  bool regfile_append_enable{false};
  /// The destination reg which needs to set dependency
  uint32_t regfile_append_id{};
  /// Current dependency id in RoB
  uint32_t regfile_append_dependency{};

  /// Flag for register write back enable
  bool regfile_write_enable{false};
  /// The destination reg to be written
  uint32_t regfile_write_id{};
  /// The value of the destination reg
  uint32_t regfile_write_val{};
  /// Current commit id in RoB
  uint32_t regfile_write_dependency{};

  /// Flag for reset of dependency (due to branch misprediction)
  bool regfile_reset{false};

  /// Whether the writeback value from ALU is on the Common Data Bus
  bool writeback1_enable{false};
  /// Id of the virtual register
  uint32_t writeback1_id{};
  /// Value of the virtual register
  uint32_t writeback1_val{};

  /// Whether to append instruction into the reservation station
  bool rs_append_enable{false};
  /// Id of the virtual destination register
  uint32_t rs_append_dest{};
  /// The ALU mode of the instruction
  ALUType rs_append_alutype{};
  /// Whether the first operand is ready
  bool rs_append_operand1_ready{};
  /// Whether the second operand is ready
  bool rs_append_operand2_ready{};
  /// The val of the first operand
  uint32_t rs_append_operand1{};
  /// The val of the second operand
  uint32_t rs_append_operand2{};

  /// Flag for clearing reservation station (due to misprediction of branch)
  bool rs_reset{false};

  /// Whether the writeback value from LSB is on the Common Data Bus
  bool writeback2_enable{false};
  /// Id of the virtual register
  uint32_t writeback2_id{};
  /// Value of the virtual register
  uint32_t writeback2_val{};

  /// Whether to append memory operation to Load Store Buffer
  bool lsb_append_enable{false};
  /// Memory visit type
  bool lsb_append_is_load{};
  /// The width of load/store
  Memory::Type lsb_append_width{};
  /// The extension mode
  bool lsb_append_sign_ext{};
  /// Whether the base register is ready
  bool lsb_append_base_ready{};
  /// Base register for memory address
  uint32_t lsb_append_base_reg{};
  /// Offset of the memory address
  uint32_t lsb_append_imm{};
  /// Whether the value register is ready
  bool lsb_append_val_ready{};
  /// Virtual destination register for LOAD op; Value to be written for STORE op
  uint32_t lsb_append_val{};

  /// Whether the first STORE in the queue is committed
  bool lsb_store_ready{false};

  /// Whether to enable instruction fetch in Load Store Buffer
  bool lsb_instruction_load_enable{false};
  /// The address of the instruction
  uint32_t lsb_instruction_load_addr{};
  /// Instruction fetched from Load Store Buffer
  uint32_t lsb_instruction{};

  /// Flag for clearing Load Store Buffer
  bool lsb_reset{false};

  /// Whether input to Reorder buffer is enabled
  bool rob_append_enable{false};
  /// Immediate type of the instruction
  ImmType rob_append_imm_type{};
  /// Operation type of the instruction
  OpType rob_append_op_type{};
  /// Whether the destination/source register is ready
  bool rob_append_operand1_ready{false};
  /// The real register id for desination reg and virtual register id for source reg
  uint32_t rob_append_operand1_id{};
  /// The val of the register
  uint32_t rob_append_operand1_val{};
  /// Whether the address/source register is ready
  bool rob_append_operand2_ready{false};
  uint32_t rob_append_operand2_id{};
  uint32_t rob_append_operand2_val{};
  /// The result of the branch predictor, true for jump
  bool rob_append_predict_branch{};
  /// The address of this instruction, used for feedback to branch predictor
  uint32_t rob_append_instruction_address{};
  /// The address of PC if we takes the branch
  uint32_t rob_append_branch_address{};
  /// The type of branch
  BrType rob_append_branch_type{};

  /// Flag for clearing reorder buffer
  bool rob_reset{};

  /// Whether the result of a B Type instruction is outputted
  bool rob_branch_result_enable{};
  /// The address of the operation
  uint32_t rob_branch_result_instruction_address{};
  /// Whether the branch is taken
  bool rob_branch_result_take_branch{};
  /// Whether branch prediction fails
  bool rob_branch_result_predict_fail{};

  /// Whether to change program counter after a branch prediction failure/JALR operation
  bool rob_set_pc_enable{};
  /// The PC address after the instruction
  uint32_t rob_set_pc{};
};

inline Wire wire_in;
inline Wire wire_out;
