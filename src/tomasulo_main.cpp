#include <iostream>

#include "reg_file/register_status.h"

int main() {
  RegisterStatus regs;
  wire_out.regfile_append_enable = true;
  wire_out.regfile_append_id = 1;
  wire_out.regfile_append_dependency = 114514;
  wire_out.regfile_write_enable = wire_out.regfile_reset = false;
  auto ret1 = regs.Query(1, 5);
  std::cout << ret1.reg1_busy << " " << ret1.reg1_val << "; " << ret1.reg2_busy << " " << ret1.reg2_val << std::endl;
  regs.Tick();

  wire_out.regfile_write_enable = true;
  wire_out.regfile_write_id = 1;
  wire_out.regfile_write_val = 1919810;
  wire_out.regfile_write_dependency = 114514;
  wire_out.regfile_append_enable = wire_out.regfile_reset = false;
  auto ret2 = regs.Query(1, 2);
  std::cout << ret2.reg1_busy << " " << ret2.reg1_val << "; " << ret2.reg2_busy << " " << ret2.reg2_val << std::endl;
  regs.Tick();
}