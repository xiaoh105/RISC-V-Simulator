#include <bits/stdc++.h>
#include <memory/decoder.h>

#include "brute_cpu.h"

int main() {
  freopen("std.txt", "w", stdout);
  std::ios::sync_with_stdio(false);
  Decoder decoder(std::cin);
  BruteCPU cpu(decoder.GetData());
  while (true) {
    cpu.Execute();
    cpu.Tick();
  }
}
