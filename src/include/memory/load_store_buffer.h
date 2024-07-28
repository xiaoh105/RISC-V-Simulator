#pragma once

#include <cstdint>

#include "memory/memory.h"

constexpr int CLOCK_PER_MEMOP = 3;

struct Entry {
  /// Whether the entry is empty
  bool busy{false};
  /// Operation type
  bool is_load{};
  bool addr_ready{};
  uint32_t base_reg{};
  /// When base register is not ready, address holds the offset to the baseR
  uint32_t address{};
  /// For LOAD, val_ready = 1 if dest reg is ready; For STORE, val_ready = 1 if it's committed.
  bool val_ready{};
  /// Holds the virtual register before the value is calculated
  uint32_t value{};
};

class LoadStoreBuffer {
public:
  LoadStoreBuffer() = delete;
  explicit LoadStoreBuffer(const std::unordered_map<uint32_t, std::byte> &memory);
  void Tick();
  [[nodiscard]] bool IsFull() const;

private:
  Memory memory_;
  /// Use a circulating array to simulate a real queue.
  Entry entries_[32]{};
  int head_{};
  /// Tail should be just a guard without actually holding value at its position.
  int tail_{};
  /// The progress of a memory access, -1 for idle
  int turn_;
};
