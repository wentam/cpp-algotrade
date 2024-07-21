#include "lib/types.hpp"
#include <string>
#include <stdexcept>
#include <cpr/cpr.h>

namespace algotrade {
  struct AlpacaAccountInfo {
    std::string id;
    // TODO admin_configurations
    // TODO user_configurations
    std::string accountNumber;
    std::string status;
    std::string cryptoStatus;
    int64_t optionsApprovedLevel;
    std::string currencyType;
    currency buyingPower;
    currency regtBuyingPower;
    currency dayTradingBuyingPower;
    currency effectiveBuyingPower;
    currency nonMarginableBuyingPower;
    currency optionsBuyingPower;
    // TODO bod_dtbp
    currency cash;
    currency accruedFees;
    currency portfolioValue;
    bool patternDayTrader;
    bool tradingBlocked;
    bool transfersBlocked;
    bool accountBlocked;
    // TODO created_at
    bool tradeSuspendedByUser;
    // TODO multiplier
    bool shortingEnabled;
    currency equity;
    currency lastEquity;
    currency longMarketValue;
    currency shortMarketValue;
    currency positionMarketValue;
    currency initialMargin;
    currency maintenanceMargin;
    currency lastMaintenanceMargin;
    // TODO sma
    int64_t daytradeCount;
    // TODO balance_asof
    // TODO crypto_tier
    // TODO intraday_adjustments
    // TODO pending_reg_taf_fees
  };

  struct AlpacaClock {
    // TODO timestamp
    bool isOpen;
    // TODO next_open
    // TODO next_close
  };

  class AlpacaAuthenticationFailure : public std::exception {
    std::string whatstr = "Alpaca authentication failed";
    const char* what() const throw() { return whatstr.c_str(); }
  };


  class UnknownAlpacaError : public std::exception {
    std::string whatstr = "Unknown alpaca error";
    const char* what() const throw() { return whatstr.c_str(); }
  };

  class AlpacaApiClient {
    public:
      // log can be NULL if you don't want logging
      AlpacaApiClient(std::string key, std::string secret, bool paperMode, FILE* log);

      AlpacaAccountInfo accountInfo();
      AlpacaClock clock();

    private:
      std::string key;
      std::string secret;
      bool paperMode;
      FILE* log;
      std::string baseUrl;
      cpr::Header authHeaders;
  };
}
