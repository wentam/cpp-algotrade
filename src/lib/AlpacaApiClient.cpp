#include "lib/AlpacaApiClient.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using namespace algotrade;
using json = nlohmann::json;

AlpacaApiClient::AlpacaApiClient(std::string key, std::string secret, bool paperMode, FILE* log) : key(key), secret(secret), paperMode(paperMode), log(log) {

  if (paperMode) {
    this->baseUrl = "https://paper-api.alpaca.markets";
  } else {
    this->baseUrl = "https://api.alpaca.markets";
  }

  this->authHeaders = cpr::Header{{"APCA-API-KEY-ID", this->key},
                                  {"APCA-API-SECRET-KEY", this->secret}};

  if (this->log) fprintf(this->log, " --- Begin logging for api client ---\n");
}

static void throwCommonErrors(cpr::Response r) {
  if (r.status_code == 403) throw AlpacaAuthenticationFailure();
}

AlpacaClock AlpacaApiClient::clock() {
  cpr::Response r = cpr::Get(cpr::Url{this->baseUrl+"/v2/clock"}, this->authHeaders);

  throwCommonErrors(r);
  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);

  AlpacaClock clock = {
    // TODO timestamp
    .isOpen = data["is_open"],
    // TODO next_open
    // TODO next_close
  };

  if (this->log) fprintf(this->log, " --- Response (status %ld):\n %s\n", r.status_code, r.text.c_str());

  return clock;
}

AlpacaAccountInfo AlpacaApiClient::accountInfo() {
  cpr::Response r = cpr::Get(cpr::Url{this->baseUrl+"/v2/account"}, this->authHeaders);

  // TODO handle cpr exceptions, throwing our own.

  throwCommonErrors(r);
  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);

  AlpacaAccountInfo info = {
    .id = data["id"],
    // TODO: admin_configurations
    // TODO: user_configurations
    .accountNumber            = data["account_number"],
    .status                   = data["status"],
    .cryptoStatus             = data["crypto_status"],
    .optionsApprovedLevel     = data["options_approved_level"],
    .currencyType             = data["currency"],
    .buyingPower              = currency(std::string(data["buying_power"])),
    .regtBuyingPower          = currency(std::string(data["regt_buying_power"])),
    .dayTradingBuyingPower    = currency(std::string(data["daytrading_buying_power"])),
    .effectiveBuyingPower     = currency(std::string(data["effective_buying_power"])),
    .nonMarginableBuyingPower = currency(std::string(data["non_marginable_buying_power"])),
    .optionsBuyingPower       = currency(std::string(data["options_buying_power"])),
    // TODO bod_dtbp
    .cash                     = currency(std::string(data["cash"])),
    .accruedFees              = currency(std::string(data["accrued_fees"])),
    .portfolioValue           = currency(std::string(data["portfolio_value"])),
    .patternDayTrader         = data["pattern_day_trader"],
    .tradingBlocked           = data["trading_blocked"],
    .transfersBlocked         = data["transfers_blocked"],
    .accountBlocked           = data["account_blocked"],
    // TODO created_at
    .tradeSuspendedByUser     = data["trade_suspended_by_user"],
    // TODO multiplier
    .shortingEnabled          = data["shorting_enabled"],
    .equity                   = currency(std::string(data["equity"])),
    .lastEquity               = currency(std::string(data["last_equity"])),
    .longMarketValue          = currency(std::string(data["long_market_value"])),
    .shortMarketValue         = currency(std::string(data["short_market_value"])),
    .positionMarketValue      = currency(std::string(data["position_market_value"])),
    .initialMargin            = currency(std::string(data["initial_margin"])),
    .maintenanceMargin        = currency(std::string(data["maintenance_margin"])),
    .lastMaintenanceMargin    = currency(std::string(data["last_maintenance_margin"])),
    .daytradeCount            = data["daytrade_count"],
    // TODO balance_asof
    // TODO crypto_tier
    // TODO intraday_adjustments
    // TODO pending_reg_taf_fees
  };

  if (this->log) fprintf(this->log, " --- Response (status %ld):\n %s\n", r.status_code, r.text.c_str());
  return info;
}
