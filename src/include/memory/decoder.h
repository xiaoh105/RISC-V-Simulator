#pragma once
#include <fstream>
#include <string>
#include <unordered_map>

class Decoder {
  using memory_type = std::unordered_map<uint32_t, std::byte>;
public:
  Decoder() = delete;
  explicit Decoder(const std::string &file_name);
  const memory_type &GetData() const;

private:
  void LoadData();
  std::fstream io_;
  memory_type memory_;
};
