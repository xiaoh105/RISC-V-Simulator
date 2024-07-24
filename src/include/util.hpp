#pragma once

#include <cstdint>

inline uint32_t FetchBits(uint32_t data, int begin, int end) {
  data >>= begin;
  data &= (1 << end - begin + 1) - 1;
  return data;
}

inline uint32_t SignExtend(uint32_t data, int len) {
  auto sign = data & 1 << len - 1;
  if (sign) {
    auto mask = UINT32_MAX ^ (1 << len) - 1;
    data |= mask;
  }
  return data;
}

inline uint32_t FetchBitsSigned(uint32_t data, int begin, int end) {
  data = FetchBits(data, begin, end);
  return SignExtend(data, end - begin + 1);
}
