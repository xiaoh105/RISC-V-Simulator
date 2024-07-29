#pragma once
#include <cstdint>

class SaturatingPredictor {
public:
  SaturatingPredictor() = default;
  [[nodiscard]] bool PredictBranch(uint32_t addr) const;
  void Tick();

private:
  uint32_t state_{0b10};
};

class TournamentPredictor {
public:
  TournamentPredictor() = default;
  [[nodiscard]] bool PredictBranch(uint32_t addr) const;
  void Tick();

private:
  uint32_t global_predictor_[1024]{};
  uint32_t local_predictor_[1024]{};
  /// >= 2 for local predictor, < 2 for global predictor
  uint32_t selector_[1024]{};
  uint32_t history_{};
};
