#include <iostream>

#include "brute_cpu.h"
#include "util.hpp"

union Word {
  int32_t sign;
  uint32_t unsign;
};

BruteCPU::BruteCPU(const std::unordered_map<uint32_t, std::byte> &mem)
: memory_(1, mem) {}

void BruteCPU::Tick() {
  alu_.Tick();
  imm_gen_.Tick();
  decoder_.Tick();
  memory_.Tick();
  reg_file_.Tick();
  ++clock_cnt_;
}

void BruteCPU::Execute() {
  auto instruction = memory_.Load(program_counter_, Memory::Type::word, true);
  if (instruction == 0x0ff00513) {
    std::cout << (reg_file_.Load(10) & 255u) << std::endl;
    exit(0);
  }
  std::cout << std::hex << program_counter_ << std::endl;
  decoder_.Decode(instruction);
  auto alu_type = decoder_.GetALUType();
  auto op_type = decoder_.GetOpType();
  auto imm_type = decoder_.GetImmType();
  auto rs1 = FetchBits(instruction, 15, 19);
  auto rs2 = FetchBits(instruction, 20, 24);
  auto rd = FetchBits(instruction, 7, 11);
  auto a = reg_file_.Load(rs1);
  auto b = reg_file_.Load(rs2);
  if (imm_type == ImmType::R) {
    auto val = alu_.Calculate(a, b, alu_type);
    reg_file_.Store(rd, val);
    program_counter_ += 4;
  } else if (imm_type == ImmType::I || imm_type == ImmType::IShift) {
    b = imm_gen_.FetchImmediate(instruction, imm_type, alu_type);
    auto val = alu_.Calculate(a, b, alu_type);
    if (op_type == OpType::Arith) {
      reg_file_.Store(rd, val);
      program_counter_ += 4;
    } else if (op_type == OpType::MemRead) {
      uint32_t sign_ext = FetchBits(instruction, 14, 14);
      uint32_t width = FetchBits(instruction, 12, 13);
      val = memory_.Load(val, static_cast<Memory::Type>(width), sign_ext);
      reg_file_.Store(rd, val);
      program_counter_ += 4;
    } else if (op_type == OpType::Branch) {
      reg_file_.Store(rd, program_counter_ + 4);
      program_counter_ = val;
    } else {
      std::cerr << "Invalid OpType in I Type" << std::endl;
      throw std::runtime_error("Invalid operation");
    }
  } else if (imm_type == ImmType::S) {
    auto imm = imm_gen_.FetchImmediate(instruction, imm_type, alu_type);
    auto addr = alu_.Calculate(a, imm, alu_type);
    auto width = FetchBits(instruction, 12, 14);
    memory_.Store(addr, b, static_cast<Memory::Type>(width));
    program_counter_ += 4;
  } else if (imm_type == ImmType::B) {
    auto branch = decoder_.GetBranchType();
    auto imm = imm_gen_.FetchImmediate(instruction, imm_type, alu_type);
    auto val = alu_.Calculate(program_counter_, imm, alu_type);
    bool flag;
    Word lhs{}, rhs{};
    lhs.unsign = a;
    rhs.unsign = b;
    switch (branch) {
      case BrType::Eq:
        flag = a == b;
        break;
      case BrType::Ne:
        flag = a != b;
        break;
      case BrType::GeU:
        flag = a >= b;
        break;
      case BrType::LtU:
        flag = a < b;
        break;
      case BrType::Ge:
        flag = lhs.sign >= rhs.sign;
        break;
      case BrType::Lt:
        flag = lhs.sign < rhs.sign;
        break;
      default:
        std::cerr << "Invalid branch type when comparing" << std::endl;
        throw std::runtime_error("Invalid branch type");
    }
    program_counter_ = flag ? val : program_counter_ + 4;
  } else if (imm_type == ImmType::U) {
    auto imm = imm_gen_.FetchImmediate(instruction, imm_type, alu_type);
    auto val = alu_.Calculate(program_counter_, imm, alu_type);
    reg_file_.Store(rd, val);
    program_counter_ += 4;
  } else if (imm_type == ImmType::J) {
    auto imm = imm_gen_.FetchImmediate(instruction, imm_type, alu_type);
    reg_file_.Store(rd, program_counter_ + 4);
    program_counter_ = alu_.Calculate(program_counter_, imm, alu_type);
  }
}
