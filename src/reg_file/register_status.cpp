#include <iostream>

#include "reg_file/register_status.h"

void RegisterStatus::Tick() {
  if (wire_out.regfile_reset && (wire_out.regfile_append_enable || wire_out.regfile_write_enable)) {
    std::cerr << "Regfile receives both input signal and reset signal" << std::endl;
    throw std::runtime_error("Invalid signal at Register Status");
  }
  if (wire_out.regfile_reset) {
    for (bool &i : busy_) {
      i = false;
    }
    reg_file_.Tick();
    return;
  }
  if (wire_out.regfile_write_enable) {
    reg_file_.Store(wire_out.regfile_write_id, wire_out.regfile_write_val);
    if (busy_[wire_out.regfile_write_id] == wire_out.regfile_write_dependency) {
      busy_[wire_out.regfile_write_id] = false;
    }
  }
  if (wire_out.regfile_append_enable) {
    busy_[wire_out.regfile_append_id] = true;
    dependency_[wire_out.regfile_append_id] = wire_out.regfile_append_dependency;
  }
  reg_file_.Tick();
}

ReplyRegister RegisterStatus::Query(uint32_t reg1_id, uint32_t reg2_id) {
  if (reg1_id >= 32 || reg2_id >= 32) {
    std::cerr << "Invalid register id when requesting register status" << std::endl;
    throw std::runtime_error("Invalid register status");
  }
  ReplyRegister ret{};
  ret.reg1_busy = busy_[reg1_id];
  ret.reg2_busy = busy_[reg2_id];
  ret.reg1_val = reg_file_.Load(reg1_id);
  ret.reg2_val = reg_file_.Load(reg2_id);
  if (wire_out.regfile_append_enable && wire_out.regfile_append_id == reg1_id) {
    ret.reg1_busy = true;
  } else if (wire_out.regfile_write_enable && wire_out.regfile_write_id == reg1_id) {
    ret.reg1_busy = false;
    ret.reg1_val = wire_out.regfile_write_val;
  }
  if (wire_out.regfile_append_enable && wire_out.regfile_append_id == reg2_id) {
    ret.reg2_busy = true;
  } else if (wire_out.regfile_write_enable && wire_out.regfile_write_id == reg2_id) {
    ret.reg2_busy = false;
    ret.reg2_val = wire_out.regfile_write_val;
  }
  if (wire_out.regfile_reset) {
    ret.reg1_busy = false;
    ret.reg2_busy = false;
  }
  return ret;
}
