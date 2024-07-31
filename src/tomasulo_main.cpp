#include <iostream>

#include "memory/decoder.h"
#include "tomasulo_cpu.h"


int main() {
  // freopen("output.txt", "w", stdout);
  std::ios::sync_with_stdio(false);
  Decoder decoder(std::cin);
  TomasuloCPU cpu(decoder.GetData());
  while (true) {
    cpu.Tick();
  }
}
