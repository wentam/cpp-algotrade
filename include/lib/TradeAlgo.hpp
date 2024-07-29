#pragma once
#include "lib/types.hpp"

namespace algotrade {
  // Trade algorithms should track their state in the sqlite database
  // provided through the incoming call to setup(). The TradeAlgo
  // is responsible for the creation, versioning, and migration of the
  // sqlite data.
  //
  // This database will be an in-memory sqlite database for simulations.
  class TradeAlgo {
    public:
      // TODO virtual void setup(sqliteDB something)
      virtual void tick(unix_time_seconds tickTime) = 0;

  };
}
