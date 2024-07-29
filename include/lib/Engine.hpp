#pragma once
#include <cstdint>

namespace algotrade {
  enum Brokerage {
    SIMULATED = 0,
    ALPACA    = 1
  };

  class Engine {
    public:
      Engine(uint64_t secondsPerTick, Brokerage broker);
      void run();

      uint64_t secondsPerTick;
      Brokerage broker;
      bool simulate;
  };
}
