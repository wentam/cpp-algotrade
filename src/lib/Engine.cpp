#include "lib/Engine.hpp"
#include "lib/types.hpp"
#include <unistd.h>
#include <cstdio>
#include <ctime>

using namespace algotrade;

Engine::Engine(uint64_t secondsPerTick, Brokerage broker) : secondsPerTick(secondsPerTick), broker(broker) {
  this->simulate = (broker == Brokerage::SIMULATED);
}

void Engine::run() {
  unix_time_seconds last_tick = 0;
  while (true) {
    fprintf(stderr, "tick\n");

    unix_time_seconds tick_time_elapsed = time(NULL)-last_tick;
    if (tick_time_elapsed < this->secondsPerTick && !this->simulate)
      sleep(this->secondsPerTick-tick_time_elapsed);
    last_tick = time(NULL);
  }
}
