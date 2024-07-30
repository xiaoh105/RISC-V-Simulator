#pragma once

#include <cstdint>

#include "memory/memory.h"

constexpr int CLOCK_PER_MEMOP = 3;

struct LSBEntry {
  /// Operation type
  bool is_load{};
  /// For LOAD, ready = 1 holds; For STORE, ready = 1 if it's committed
  bool ready{false};
  bool addr_ready{};
  uint32_t base_reg{};
  /// When base register is not ready, address holds the offset to the baseR
  uint32_t address{};
  /// For LOAD, val_ready = 1 holds; For STORE, val_ready = 1 if the value is ready.
  bool val_ready{};
  /// The width of the memory op
  Memory::Type width{};
  /// Whether the memory operation is sign extended
  bool sign_ext{};
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
  LSBEntry entries_[32]{};
  int head_{0};
  /// Tail should be just a guard without actually holding value at its position.
  int tail_{0};
  /// The progress of a memory access, -1 for idle
  int turn_{-1};
};
