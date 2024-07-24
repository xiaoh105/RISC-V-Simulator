#include <iostream>

#include "memory/decoder.h"

std::byte DecodeByte(const std::string &data) {
  std::size_t index;
  auto value = std::stoul(data, &index, 16);
  if (index != 2) {
    std::cerr << "Error occurred when transforming  " << data << " to byte" << std::endl;
    throw std::runtime_error("Invalid input");
   }
  return static_cast<std::byte>(value);
}

uint32_t DecodeAddress(const std::string &data) {
  std::size_t index;
  auto value = std::stoul(data, &index, 16);
  if (index != 8) {
    std::cerr << "Error occurred when transforming  " << data << " to address" << std::endl;
    throw std::runtime_error("Invalid input");
  }
  return static_cast<uint32_t>(value);
}

Decoder::Decoder(const std::string& file_name) {
  io_.open(file_name);
  if (!io_.is_open()) {
    std::cerr << "Can not open data file " << file_name << std::endl;
    throw std::runtime_error("Cannot open file");
  }
  LoadData();
}

const Decoder::memory_type &Decoder::GetData() const {
  return memory_;
}


void Decoder::LoadData() {
  uint32_t address{};
  while (!io_.eof()) {
    std::string s;
    io_ >> s;
    if (s.empty()) {
      continue;
    }
    if (s[0] == '@') {
      address = DecodeAddress(s.substr(1));
    } else {
      auto val = DecodeByte(s);
      memory_[address++] = val;
    }
  }
}

