#pragma once

#include "reg_file/reg_file.h"

struct ReplyRegister {
  bool reg1_busy;
  uint32_t reg1_val;
  bool reg2_busy;
  uint32_t reg2_val;
};

class RegisterStatus {
public:
  RegisterStatus() = default;
  void Tick();
  [[nodiscard]] ReplyRegister Query(uint32_t reg1_id, uint32_t reg2_id);

private:
  /// Whether the register has dependency.
  bool busy_[32]{};
  /// Which index of RoB is this register dependent on.
  uint32_t dependency_[32]{};
  /// Real register file under abstraction.
  RegFile reg_file_;
};