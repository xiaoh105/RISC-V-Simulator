#include <bits/stdc++.h>
#include <memory/decoder.h>

#include "brute_cpu.h"

int main() {
  std::ifstream io;
  io.open("../testcases/pi.data");
  if (!io.is_open()) {
    std::cerr << "Cannot open .data file" << std::endl;
    throw std::runtime_error("Invalid input stream");
  }
  Decoder decoder(io);
  BruteCPU cpu(decoder.GetData());
  while (true) {
    cpu.Execute();
    cpu.Tick();
  }
}
