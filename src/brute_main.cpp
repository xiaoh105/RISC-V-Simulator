#include <bits/stdc++.h>
#include <memory/decoder.h>

#include "brute_cpu.h"

int main() {
  Decoder decoder(std::cin);
  BruteCPU cpu(decoder.GetData());
  while (true) {
    cpu.Execute();
    cpu.Tick();
  }
}
