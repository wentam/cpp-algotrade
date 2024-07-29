#pragma once
#include "lib/types.hpp"
#include "lib/AlpacaApiClient.hpp"

namespace algotrade {
  class BrokerIO {
    public:

      enum TIF {
        DAY = 0,
        GOOD_TILL_CANCELLED = 1,
      };

      struct AccountInfo {
        currency balance;
        currency equity;
        int32_t remainingDayTrades;
        bool canShort;
      };

      virtual AccountInfo accountInfo() = 0;

      // Limit price of -1 means market order
      // positive quantity means buy, negative quantity means sell
      virtual void placeOrder(std::string asset,
          int64_t qty,
          currency limitPrice,
          TIF tif,
          bool extendedHours) = 0;
  };

  class AlpacaBrokerIO : public BrokerIO {
    public:
      AlpacaBrokerIO(AlpacaApiClient* alpaca);
      AccountInfo accountInfo();
      void placeOrder(std::string asset,
          int64_t qty,
          currency limitPrice,
          TIF tif,
          bool extendedHours);

    private:
      AlpacaApiClient* alpaca;
  };
}
