#include <iostream>

#include "memory/load_store_buffer.h"
#include "wiring.h"

LoadStoreBuffer::LoadStoreBuffer(const std::unordered_map<uint32_t, std::byte>& memory)
  : memory_(3, memory){}

void LoadStoreBuffer::Tick() {
  if (wire_out.lsb_reset && wire_out.lsb_store_ready) {
    std::cerr << "Both the reset signal and store ready signal is set in LSB" << std::endl;
    throw std::runtime_error("Invalid signal");
  }
  wire_in.writeback2_enable = false;
  wire_in.lsb_instruction_return_enable = false;
  if (wire_out.writeback1_enable) {
    for (auto &entry : entries_) {
      if (!entry.addr_ready && entry.base_reg == wire_out.writeback1_id) {
        entry.addr_ready = true;
        entry.address += wire_out.writeback1_val;
      }
      if (!entry.val_ready && entry.value == wire_out.writeback1_id) {
        entry.val_ready = true;
        entry.value = wire_out.writeback1_val;
      }
    }
  }
  if (wire_out.writeback2_enable) {
    for (auto &entry : entries_) {
      if (!entry.addr_ready && entry.base_reg == wire_out.writeback2_id) {
        entry.addr_ready = true;
        entry.address += wire_out.writeback2_val;
      }
      if (!entry.val_ready && entry.value == wire_out.writeback2_id) {
        entry.val_ready = true;
        entry.value = wire_out.writeback2_val;
      }
    }
  }
  if (wire_out.lsb_store_ready) {
    auto cur = head_;
    bool flag = false;
    while (cur != tail_) {
      if (!entries_[cur].ready) {
        entries_[cur].ready = true;
        flag = true;
        break;
      }
      ++cur;
      if (cur == 32) {
        cur = 0;
      }
    }
    if (!flag) {
      std::cerr << "No valid uncommitted entry is found" << std::endl;
      throw std::runtime_error("Invalid store ready signal");
    }
  }
  if (turn_ != -1) {
    ++turn_;
    if (turn_ == CLOCK_PER_MEMOP) {
      if (entries_[head_].is_load) {
        auto val = memory_.Load(entries_[head_].address, entries_[head_].width, entries_[head_].sign_ext);
        wire_in.writeback2_enable = true;
        wire_in.writeback2_id = entries_[head_].value;
        wire_in.writeback2_val = val;
      }
      ++head_;
      if (head_ == 32) {
        head_ = 0;
      }
      turn_ = -1;
    }
    memory_.Tick();
  } else {
    if (head_ != tail_ && entries_[head_].addr_ready && entries_[head_].val_ready && entries_[head_].ready) {
      turn_ = 0;
      if (!entries_[head_].is_load) {
        memory_.Store(entries_[head_].address, entries_[head_].value, entries_[head_].width);
      }
    }
  }
  if (wire_out.lsb_append_enable) {
    if ((tail_ + 1) % 32 == head_) {
      std::cerr << "LSB receives append signal when it's already full" << std::endl;
      throw std::runtime_error("Invalid append signal");
    }
    entries_[tail_].addr_ready = wire_out.lsb_append_base_ready;
    entries_[tail_].ready = wire_out.lsb_append_ready;
    entries_[tail_].base_reg = wire_out.lsb_append_base_reg;
    entries_[tail_].address = wire_out.lsb_append_imm;
    entries_[tail_].is_load = wire_out.lsb_append_is_load;
    entries_[tail_].val_ready = wire_out.lsb_append_val_ready;
    entries_[tail_].value = wire_out.lsb_append_val;
    entries_[tail_].width = wire_out.lsb_append_width;
    entries_[tail_].sign_ext = wire_out.lsb_append_sign_ext;
    tail_ = (tail_ + 1) % 32;
  }
  if (wire_out.lsb_instruction_load_enable) {
    wire_in.lsb_instruction_return_enable = true;
    wire_in.lsb_instruction_return = memory_.Load(wire_out.lsb_instruction_load_addr, Memory::Type::word, false);
    wire_in.lsb_instruction_return_addr = wire_out.lsb_instruction_load_addr;
  }
  if (wire_out.lsb_reset) {
    while (head_ != tail_) {
      auto id = tail_ == 0 ? 31 : tail_ - 1;
      if (entries_[id].is_load || !entries_[id].val_ready) {
        --tail_;
        if (tail_ == -1) {
          tail_ = 31;
        }
      }
    }
  }
  memory_.Tick();
}

bool LoadStoreBuffer::IsFull() const {
  return (tail_ + 1) % 32 == head_ || (tail_ + 2) % 32 == head_;
}
