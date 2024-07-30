#include <iostream>

#include "alu/reservation_station.h"
#include "wiring.h"

void ReservationStation::Tick() {
  if (wire_out.rs_reset) {
    for (auto &entry : entries_) {
      entry.busy = false;
    }
    size_ = 0;
    return;
  }
  if (size_ >= 32 && wire_out.rs_append_enable) {
    std::cerr << "Invalid append signal: The reservation station is full" << std::endl;
    throw std::runtime_error("Invalid append signal");
  }
  if (wire_out.rs_append_enable) {
    for (auto &entry : entries_) {
      if (!entry.busy) {
        entry.busy = true;
        entry.alu_type = wire_out.rs_append_alutype;
        entry.dest = wire_out.rs_append_dest;
        entry.operand1_ready = wire_out.rs_append_operand1_ready;
        entry.operand2_ready = wire_out.rs_append_operand2_ready;
        entry.operand1_val = wire_out.rs_append_operand1;
        entry.operand2_val = wire_out.rs_append_operand2;
        ++size_;
        break;
      }
    }
  }
  if (wire_out.writeback1_enable) {
    for (auto &entry : entries_) {
      if (!entry.operand1_ready && entry.operand1_val == wire_out.writeback1_id) {
        entry.operand1_ready = true;
        entry.operand1_val = wire_out.writeback1_val;
      }
      if (!entry.operand2_ready && entry.operand2_val == wire_out.writeback1_id) {
        entry.operand2_ready = true;
        entry.operand2_val = wire_out.writeback1_val;
      }
    }
  }
  if (wire_out.writeback2_enable) {
    for (auto &entry : entries_) {
      if (!entry.operand1_ready && entry.operand1_val == wire_out.writeback2_id) {
        entry.operand1_ready = true;
        entry.operand1_val = wire_out.writeback2_val;
      }
      if (!entry.operand2_ready && entry.operand2_val == wire_out.writeback2_id) {
        entry.operand2_ready = true;
        entry.operand2_val = wire_out.writeback2_val;
      }
    }
  }
  wire_in.writeback1_enable = false;
  for (auto &entry : entries_) {
    if (entry.busy && entry.operand1_ready && entry.operand2_ready) {
      auto ret = alu_.Calculate(entry.operand1_val, entry.operand2_val, entry.alu_type);
      wire_in.writeback1_enable = true;
      wire_in.writeback1_id = entry.dest;
      wire_in.writeback1_val = ret;
      entry.busy = false;
      --size_;
      break;
    }
  }

}

bool ReservationStation::IsFull() const {
  return size_ + 1 >= 32;
}
