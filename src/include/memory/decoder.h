#pragma once
#include <fstream>
#include <unordered_map>

class Decoder {
  using memory_type = std::unordered_map<uint32_t, std::byte>;
public:
  Decoder() = delete;
  explicit Decoder(std::istream &io);
  const memory_type &GetData() const;

private:
  void LoadData();
  std::istream &io_;
  memory_type memory_;
};
