#pragma once
#include <cstdint>

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

};

inline Wire wire_in;
inline Wire wire_out;