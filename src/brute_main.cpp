#include <bits/stdc++.h>
#include "memory/memory.h"
#include "memory/decoder.h"
#include "decoder/instruction_decoder.h"
#include "util.hpp"
union Word {
  int32_t sign;
  uint32_t unsign;
};

int main() {
  Word tmp{};
  tmp.unsign = FetchBitsSigned(0b111111011, 0, 8);
  std::cout << tmp.sign << std::endl;
}
