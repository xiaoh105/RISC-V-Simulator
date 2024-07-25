#include <iostream>

#include "decoder/instruction_decoder.h"
#include "util.hpp"

void InstructionDecoder::Decode(uint32_t instruction) {
  if (decoded_) {
    std::cerr << "Calling decode twice in a single clock cycle" << std::endl;
    throw std::runtime_error("Invalid decode operation");
  }
  instruction_ = instruction;
  SetOpType();
  SetImmType();
  SetALUType();
  SetBranchType();
}

void InstructionDecoder::Tick() {
  decoded_ = false;
}


ImmType InstructionDecoder::GetImmType() const { return imm_type_; }

OpType InstructionDecoder::GetOpType() const { return op_type_; }

ALUType InstructionDecoder::GetALUType() const { return alu_type_; }

BrType InstructionDecoder::GetBranchType() const { return branch_type_; }

void InstructionDecoder::SetOpType() {
  auto opcode = FetchBits(instruction_, 0, 6);
  if (opcode & 0b10000) {
    op_type_ = OpType::Arith;
  } else if (opcode == 0b0000011) {
    op_type_ = OpType::MemRead;
  } else if (opcode == 0b0100011) {
    op_type_ = OpType::MemWrite;
  } else {
    op_type_ = OpType::Branch;
  }
}

void InstructionDecoder::SetImmType() {
  auto opcode = FetchBits(instruction_, 0, 6);
  if (opcode == 0b0110011) {
    imm_type_ = ImmType::R;
  } else if (opcode == 0b0010011 || opcode == 0b1100111) {
    auto funct3 = FetchBits(instruction_, 12, 14);
    if (funct3 == 0b001 || funct3 == 0b101) {
      imm_type_ = ImmType::IShift;
    } else {
      imm_type_ = ImmType::I;
    }
  } else if (opcode == 0b0000011) {
    imm_type_ = ImmType::I;
  } else if (opcode == 0b0100011) {
    imm_type_ = ImmType::S;
  } else if (opcode == 0b1100011) {
    imm_type_ = ImmType::B;
  } else if (opcode == 0b1101111) {
    imm_type_ = ImmType::J;
  } else if (opcode == 0b0010111 || opcode == 0b0110111) {
    imm_type_ = ImmType::U;
  } else {
    std::cerr << "Invalid opcode " << opcode << std::endl;
    throw std::runtime_error("Invalid instruction");
  }
}

void InstructionDecoder::SetALUType() {
  auto opcode = FetchBits(instruction_, 0, 6);
  if (imm_type_ == ImmType::J || imm_type_ == ImmType::B || imm_type_ == ImmType::S) {
    alu_type_ = ALUType::Add;
  } else if (imm_type_ == ImmType::U) {
    if (opcode == 0b0010111) {
      alu_type_ = ALUType::Add;
    } else {
      alu_type_ = ALUType::UseLatter;
    }
  } else if (imm_type_ == ImmType::R) {
    auto funct3 = FetchBits(instruction_, 12, 14);
    auto funct7 = FetchBits(instruction_, 27, 31);
    if (funct3 == 0b000) {
      alu_type_ = funct7 ? ALUType::Sub : ALUType::Add;
    } else if (funct3 == 0b001) {
      alu_type_ = ALUType::ShiftL;
    } else if (funct3 == 0b010) {
      alu_type_ = ALUType::SetLTSigned;
    } else if (funct3 == 0b011) {
      alu_type_ = ALUType::SetLTUnsigned;
    } else if (funct3 == 0b100) {
      alu_type_ = ALUType::Xor;
    } else if (funct3 == 0b101) {
      alu_type_ = funct7 ? ALUType::ShiftRArith : ALUType::ShiftRLogical;
    } else if (funct3 == 0b110) {
      alu_type_ = ALUType::Or;
    } else if (funct3 == 0b111) {
      alu_type_ = ALUType::And;
    } else {
      std::cerr << "Invalid R Type funct3 " << funct3 << std::endl;
      throw std::runtime_error("Invalid instruction");
    }
  } else if (opcode == 0b0000011) {
    alu_type_ = ALUType::Add;
  } else {
    auto funct3 = FetchBits(instruction_, 12, 14);
    auto funct7 = FetchBits(instruction_, 27, 31);
    if (funct3 == 0b000) {
      alu_type_ = ALUType::Add;
    } else if (funct3 == 0b001) {
      alu_type_ = ALUType::ShiftL;
    } else if (funct3 == 0b010) {
      alu_type_ = ALUType::SetLTSigned;
    } else if (funct3 == 0b011) {
      alu_type_ = ALUType::SetLTUnsigned;
    } else if (funct3 == 0b100) {
      alu_type_ = ALUType::Xor;
    } else if (funct3 == 0b101) {
      alu_type_ = funct7 ? ALUType::ShiftRArith : ALUType::ShiftRLogical;
    } else if (funct3 == 0b110) {
      alu_type_ = ALUType::Or;
    } else if (funct3 == 0b111) {
      alu_type_ = ALUType::And;
    } else {
      std::cerr << "Invalid I Type funct3 " << funct3 << std::endl;
      throw std::runtime_error("Invalid instruction");
    }
  }
}

void InstructionDecoder::SetBranchType() {
  if (imm_type_ != ImmType::B) {
    branch_type_ = BrType::Eq;
  } else {
    auto funct3 = FetchBits(instruction_, 12, 14);
    branch_type_ = static_cast<BrType>(funct3);
  }
}
