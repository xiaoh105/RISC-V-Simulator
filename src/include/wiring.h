#pragma once
#include <cstdint>

#include "decoder/instruction_decoder.h"

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
  /// Base register for memory address
  uint32_t lsb_append_base_reg{};
  /// Offset of the memory address
  uint32_t lsb_append_imm{};
  /// Virtual destination register for LOAD op; Value to be written for STORE op
  uint32_t lsb_append_val{};

  /// Whether the first STORE in the queue is committed
  bool lsb_store_ready_{false};

  /// Flag for clearing Load Store Buffer
  bool lsb_reset{false};
};

inline Wire wire_in;
inline Wire wire_out;
