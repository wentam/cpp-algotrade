#include "lib/AlpacaApiClient.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <chrono>

using namespace algotrade;
using json = nlohmann::json;

static int64_t time_microseconds() {
  auto now_since_epoch = std::chrono::high_resolution_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::microseconds>(now_since_epoch).count();
}

AlpacaApiClient::AlpacaApiClient(std::string key, std::string secret, bool paperMode, int64_t rateLimit, FILE* log) : key(key), secret(secret), paperMode(paperMode), rpm(rateLimit), log(log) {

  this->lastRequest = 0;

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
  if (r.status_code == 429) throw AlpacaRateLimitExceeded();
}

void AlpacaApiClient::rateLimit() {
  int64_t time = time_microseconds();

  int64_t timeSinceLastRequest = time - this->lastRequest;
  int64_t timePerRequest = (60000000/this->rpm);
  int64_t timeUntilNextRequest = timePerRequest-timeSinceLastRequest;
  fprintf(stderr, "%ld\n", timeSinceLastRequest);

  if (timeUntilNextRequest > 0) usleep(timeUntilNextRequest);

  this->lastRequest = time_microseconds(); // We're about to make a request, so update lastRequest
}

cpr::Response AlpacaApiClient::apiCall(bool dataApi,
                              std::string endpoint,
                              bool post,
                              cpr::Header extraHeaders) {
  std::string base = this->baseUrl;

  if (dataApi) {
    // TODO change base
  }

  this->rateLimit();

  cpr::Response r;

  if (post) {
    // TODO
  } else {
     r = cpr::Get(cpr::Url{base+endpoint}, this->authHeaders);
  }

  // TODO handle cpr exceptions, throwing our own.

  if (this->log) fprintf(this->log, " --- Response (status %ld):\n %s\n", r.status_code, r.text.c_str());

  throwCommonErrors(r);

  return r;
}

AlpacaClock AlpacaApiClient::clock() {
  auto r = this->apiCall(false, "/v2/clock", false, {});
  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);

  return {
    // TODO timestamp
    .isOpen = data["is_open"],
    // TODO next_open
    // TODO next_close
  };
}

AlpacaAccountInfo AlpacaApiClient::accountInfo() {
  auto r = this->apiCall(false, "/v2/account", false, {});
  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);

  return {
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
}
