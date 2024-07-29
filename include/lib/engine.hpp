#pragma once
#include "lib/TradeAlgo.hpp"
#include <cstdint>

namespace algotrade {
  enum Brokerage {
    ALPACA    = 0
  };

  void run_algo(int64_t secondsPerTick, Brokerage broker, TradeAlgo& algo);
  void simulate_algo(int64_t secondsPerTick, TradeAlgo& algo, unix_time_seconds start, unix_time_seconds end);
}
