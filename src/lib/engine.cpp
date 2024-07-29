#include "lib/engine.hpp"
#include "lib/types.hpp"
#include <unistd.h>
#include <cstdio>
#include <ctime>

using namespace algotrade;

void algotrade::run_algo(int64_t secondsPerTick, Brokerage broker, TradeAlgo& algo) {
  while (true) {
    unix_time_seconds tickTime = time(NULL);

    algo.tick(tickTime);

    unix_time_seconds tick_time_elapsed = time(NULL)-tickTime;
    if (tick_time_elapsed < secondsPerTick) sleep(secondsPerTick-tick_time_elapsed);
  }
}

void algotrade::simulate_algo(int64_t secondsPerTick, TradeAlgo& algo, unix_time_seconds start, unix_time_seconds end) {
  unix_time_seconds tickTime = start;
  while(tickTime < end) {
    algo.tick(tickTime);
    tickTime += secondsPerTick;
  }
}
