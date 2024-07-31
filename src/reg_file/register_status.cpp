#include <iostream>

#include "reg_file/register_status.h"
#include "reorder_buffer/reorder_buffer.h"
#include "wiring.h"

void RegisterStatus::Tick() {
  if (wire_out.regfile_reset) {
    for (bool &i : busy_) {
      i = false;
    }
    reg_file_.Tick();
    return;
  }
  if (wire_out.regfile_write_enable) {
    if (wire_out.regfile_write_id == 10 && wire_out.regfile_write_val == 255 && terminate) {
      std::cout << std::dec << (reg_file_.Load(10) & 255) << std::endl;
      // ReportBranch();
      exit(0);
    }
    reg_file_.Store(wire_out.regfile_write_id, wire_out.regfile_write_val);
    if (dependency_[wire_out.regfile_write_id] == wire_out.regfile_write_dependency) {
      busy_[wire_out.regfile_write_id] = false;
    }
  }
  if (wire_out.regfile_append_enable) {
    if (!(wire_out.regfile_write_enable && wire_out.regfile_write_dependency == wire_out.regfile_append_dependency)) {
      busy_[wire_out.regfile_append_id] = true;
      dependency_[wire_out.regfile_append_id] = wire_out.regfile_append_dependency;
    }
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
  ret.reg1_dependency = dependency_[reg1_id];
  ret.reg2_dependency = dependency_[reg2_id];
  if (wire_out.regfile_append_enable && wire_out.regfile_append_id == reg1_id) {
    ret.reg1_busy = true;
    ret.reg1_dependency = wire_out.regfile_append_dependency;
  } else if (wire_out.regfile_write_enable && wire_out.regfile_write_id == reg1_id &&
    wire_out.regfile_write_dependency == ret.reg1_dependency) {
    ret.reg1_busy = false;
    ret.reg1_val = wire_out.regfile_write_val;
  }
  if (wire_out.regfile_append_enable && wire_out.regfile_append_id == reg2_id) {
    ret.reg2_busy = true;
    ret.reg2_dependency = wire_out.regfile_append_dependency;
  } else if (wire_out.regfile_write_enable && wire_out.regfile_write_id == reg2_id &&
    wire_out.regfile_write_dependency == ret.reg2_dependency) {
    ret.reg2_busy = false;
    ret.reg2_val = wire_out.regfile_write_val;
  }
  if (reg1_id == 0) {
    ret.reg1_busy = false;
    ret.reg1_val = 0;
  }
  if (reg2_id == 0) {
    ret.reg2_busy = false;
    ret.reg2_val = 0;
  }
  if (wire_out.regfile_reset) {
    ret.reg1_busy = false;
    ret.reg2_busy = false;
  }
  return ret;
}
