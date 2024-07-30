#pragma once

#include "alu/reservation_station.h"
#include "instruction_queue/instruction_queue.h"
#include "memory/load_store_buffer.h"
#include "reg_file/register_status.h"
#include "reorder_buffer/reorder_buffer.h"

class TomasuloCPU {
public:
  TomasuloCPU() = delete;
  explicit TomasuloCPU(const std::unordered_map<uint32_t, std::byte> &memory);
  void Tick();

private:
  InstructionQueue instruction_queue_;
  ReservationStation reservation_station_;
  LoadStoreBuffer load_store_buffer_;
  RegisterStatus register_status_;
  ReorderBuffer reorder_buffer_;

  int clock_cycle = 0;
};