#include "tomasulo_cpu.h"
#include "wiring.h"

TomasuloCPU::TomasuloCPU(const std::unordered_map<uint32_t, std::byte>& memory)
  : load_store_buffer_(memory),
    instruction_queue_(load_store_buffer_, reservation_station_,
      register_status_, reorder_buffer_){}

void TomasuloCPU::Tick() {
  instruction_queue_.Tick();
  reorder_buffer_.Tick();
  load_store_buffer_.Tick();
  reservation_station_.Tick();
  register_status_.Tick();
  wire_out = wire_in;
  ++clock_cycle;
}
