#pragma once
#include <cstdint>

enum class OpType : int {
  Arith, Branch, MemRead, MemWrite
};

enum class ImmType : int {
  R, I, IShift, S, B, U, J
};

enum class ALUType : int {
  Add, Sub,
  And, Or, Xor,
  ShiftL, ShiftRLogical, ShiftRArith,
  SetLTSigned, SetLTUnsigned,
  UseLatter
};

class InstructionDecoder {
public:
  InstructionDecoder() = default;

  void Decode(uint32_t instruction);

  [[nodiscard]] OpType GetOpType() const;

  [[nodiscard]] ImmType GetImmType() const;

  [[nodiscard]] ALUType GetALUType() const;

  void Tick();

private:
  void SetOpType();
  void SetImmType();
  /// Notice: This function must be executed AFTER SetOpType() and SetImmType()
  void SetALUType();
  uint32_t instruction_;
  OpType op_type_;
  ImmType imm_type_;
  ALUType alu_type_;
  // Debugging variables
  bool decoded_;
};
