#pragma once

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

  struct AlpacaCalendarEntry {
    std::chrono::time_point<std::chrono::system_clock> open;
    std::chrono::time_point<std::chrono::system_clock> close;
  };

  class AlpacaAuthenticationFailure : public std::exception {
    std::string whatstr = "Alpaca authentication failed";
    const char* what() const throw() { return whatstr.c_str(); }
  };

  class AlpacaRateLimitExceeded : public std::exception {
    std::string whatstr = "Alpaca api rate limit exceeded";
    const char* what() const throw() { return whatstr.c_str(); }
  };


  class UnknownAlpacaError : public std::exception {
    std::string whatstr = "Unknown alpaca error";
    const char* what() const throw() { return whatstr.c_str(); }
  };

  class AlpacaApiClient {
    public:
      AlpacaApiClient(std::string key,    // your alpaca key
                      std::string secret, // your alpaca secret
                      bool paperMode,     // true for paper trading, false for real trading
                      int64_t rateLimit,  // requests-per-minute for client-side limiting, else -1
                      FILE* log);         // log destination for api logging, NULL otherwise

      // All api calls may throw:
      // * AlpacaAuthenticationFailure
      // * AlpacaRateLimitExceeded
      // * UnknownAlpacaError

      AlpacaAccountInfo accountInfo();
      AlpacaClock       clock();

      std::vector<Bar>  bars(std::string symbol,
                             std::chrono::time_point<std::chrono::system_clock> start,
                             std::chrono::time_point<std::chrono::system_clock> end,
                             int64_t limit,          // Max bars to return, max 10,000
                             std::string timeframe); // 15Min, 2Hour, 3Day, 3Week, 2Month

      std::vector<AlpacaCalendarEntry> calendar(
          std::chrono::time_point<std::chrono::system_clock> start,
          std::chrono::time_point<std::chrono::system_clock> end
      );

    private:
      void rateLimit();
      int64_t lastRequest;

      cpr::Response apiCall(bool dataApi,
                            std::string endpoint,
                            bool post,
                            cpr::Header extraHeaders,
                            cpr::Parameters extraParameters);

      std::string key;
      std::string secret;
      bool paperMode;
      int64_t rpm;
      FILE* log;
      std::string baseUrl;
      std::string dataBaseUrl;
      cpr::Header authHeaders;
  };
}
