#include <cassert>
#include <iostream>

#include "instruction_queue/branch_predictor.h"
#include "wiring.h"

bool SaturatingPredictor::PredictBranch(uint32_t addr) const {
  return state_ >= 2;
}

void SaturatingPredictor::Tick() {
  if (wire_out.rob_branch_result_enable) {
    if (wire_out.rob_branch_result_take_branch) {
      state_ = std::min(state_ + 1, 3u);
    } else {
      state_ = state_ == 0 ? 0 : state_ - 1;
    }
  }
}

void TournamentPredictor::Tick() {
  assert(history_ <= 1023);
  if (wire_out.rob_branch_result_enable) {
    auto address = wire_out.rob_branch_result_instruction_address & (1 << 10) - 1;
    assert(address <= 1023);
    auto global_prediction = global_predictor_[history_] >= 2;
    auto local_prediction = local_predictor_[address] >= 2;
    auto branch = wire_out.rob_branch_result_take_branch;
    if (global_prediction == branch && local_prediction != branch) {
      selector_[address] = selector_[address] == 0 ? 0 : selector_[address] - 1;
    }
    if (global_prediction != branch && local_prediction == branch) {
      selector_[address] = selector_[address] == 3 ? 3u : selector_[address] + 1;
    }
    if (branch) {
      global_predictor_[history_] = global_predictor_[history_] == 3 ? 3u : global_predictor_[history_] + 1;
      local_predictor_[history_] = local_predictor_[address] == 3 ? 3u : local_predictor_[history_] + 1;
    } else {
      global_predictor_[history_] = global_predictor_[history_] == 0 ? 0u : global_predictor_[history_] - 1;
      local_predictor_[history_] = local_predictor_[address] == 0 ? 0u : local_predictor_[history_] - 1;
    }
    history_ <<= 1;
    if (wire_out.rob_branch_result_take_branch) {
      history_ |= 1;
    }
    history_ &= (1 << 10) - 1;
  }
}

bool TournamentPredictor::PredictBranch(uint32_t addr) const {
  addr &= (1 << 10) - 1;
  assert(addr <= 1023);
  assert(history_ <= 1023);
  if (selector_[addr] >= 2) {
    return local_predictor_[addr] >= 2;
  }
  return global_predictor_[history_] >= 2;
}
