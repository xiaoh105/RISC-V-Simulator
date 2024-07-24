#pragma once
#include <unordered_map>

class Memory {
public:
  enum class Type : int { byte = 1, halfword = 2, word = 4 };
  Memory() = delete;

  explicit Memory(int clock_per_visit, const std::unordered_map<uint32_t, std::byte> &memory);

  void Tick();

  uint32_t Load(uint32_t addr, Type type, bool signed_ext);

  void Store(uint32_t addr, uint32_t data, Type type);

private:
  const int clock_per_visit_;
  std::unordered_map<uint32_t, std::byte> memory_{};
  int turn_{};
  // Debug variables
  bool busy_{};
  uint32_t current_op_{};
};
