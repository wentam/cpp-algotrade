#include "lib/AlpacaApiClient.hpp"
#include "lib/util.hpp"
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

  this->dataBaseUrl = "https://data.alpaca.markets";

  this->authHeaders = cpr::Header{{"APCA-API-KEY-ID", this->key},
                                  {"APCA-API-SECRET-KEY", this->secret}};

  if (this->log) fprintf(this->log, " --- Begin logging for api client ---\n");
}

static void throwCommonErrors(cpr::Response r) {
  if (r.status_code == 403) throw AlpacaAuthenticationFailure();
  if (r.status_code == 429) throw AlpacaRateLimitExceeded();
}

void AlpacaApiClient::rateLimit() {
  if (this->rpm == -1) return;

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
                              json postData,
                              cpr::Header extraHeaders,
                              cpr::Parameters extraParameters) {
  this->rateLimit();

  std::string  base = this->baseUrl;
  if (dataApi) base = this->dataBaseUrl;

  cpr::Response r;

  if (post) {
    r = cpr::Post(cpr::Url{base+endpoint},
                  this->authHeaders,
                  // TODO extraHeaders,
                  extraParameters,
                  cpr::Body{postData.dump()});
  } else {
     r = cpr::Get(cpr::Url{base+endpoint},
                  this->authHeaders,
                  // TODO extraHeaders,
                  extraParameters);
  }

  // TODO handle cpr exceptions, throwing our own.

  if (this->log) fprintf(this->log, " --- Response (status %ld):\n %s\n", r.status_code, r.text.c_str());

  throwCommonErrors(r);

  return r;
}

AlpacaClock AlpacaApiClient::clock() {
  auto r = this->apiCall(false, "/v2/clock", false, {}, {}, {});
  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);

  return {
    // TODO timestamp
    .isOpen = data["is_open"],
    // TODO next_open
    // TODO next_close
  };
}

static AlpacaAsset assetFromJson(json data) {
  return (AlpacaAsset){
    .id                     = json::string_t(data["id"]),
    .symbol                 = json::string_t(data["symbol"]),
    .assetClass             = json::string_t(data["class"]),
    .exchange               = json::string_t(data["exchange"]),
    .name                   = json::string_t(data["name"]),
    .status                 = json::string_t(data["status"]),
    .tradable               = json::boolean_t(data["tradable"]),
    .marginable             = json::boolean_t(data["marginable"]),
    .shortable              = json::boolean_t(data["shortable"]),
    .easyToBorrow           = json::boolean_t(data["easy_to_borrow"]),
    .fractionable           = json::boolean_t(data["fractionable"]),
    .marginRequirementLong  = json::string_t(data["margin_requirement_long"]),
    .marginRequirementShort = json::string_t(data["margin_requirement_short"])
  };
}

AlpacaAsset AlpacaApiClient::asset(std::string symbol) {
  auto r = this->apiCall(false, "/v2/assets/"+symbol, false, {}, {}, {});
  if (r.status_code == 404) throw AssetNotFoundAlpacaError();
  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);
  return assetFromJson(data);
}

std::vector<AlpacaAsset> AlpacaApiClient::assets() {
  auto r = this->apiCall(false, "/v2/assets", false, {}, {}, {});
  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);

  std::vector<AlpacaAsset> result;
  for (auto asset : data) result.push_back(assetFromJson(asset));
  return result;
}

std::vector<AlpacaPosition> AlpacaApiClient::positions() {
  auto r = this->apiCall(false, "/v2/positions", false, {}, {}, {});
  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);

  std::vector<AlpacaPosition> result;

  for (auto position : data) {
    result.push_back({
        .assetId                =                   json::string_t(position["asset_id"]),
        .symbol                 =                   json::string_t(position["symbol"]),
        .exchange               =                   json::string_t(position["exchange"]),
        .assetClass             =                   json::string_t(position["asset_class"]),
        .avgEntryPrice          =          currency(json::string_t(position["avg_entry_price"])),
        .qty                    =        std::stoll(json::string_t(position["qty"])),
        .side                   =                   json::string_t(position["side"]),
        .marketValue            =          currency(json::string_t(position["market_value"])),
        .costBasis              =          currency(json::string_t(position["cost_basis"])),
        .unrealizedPl           =          currency(json::string_t(position["unrealized_pl"])),
        .unrealizedPlpc         = cpp_dec_float_100(json::string_t(position["unrealized_plpc"])),
        .unrealizedIntradayPl   =          currency(json::string_t(position["unrealized_intraday_pl"])),
        .unrealizedIntradayPlpc = cpp_dec_float_100(json::string_t(position["unrealized_intraday_plpc"])),
        .currentPrice           =          currency(json::string_t(position["current_price"])),
        .lastDayPrice           =          currency(json::string_t(position["lastday_price"])),
        .changeToday            = cpp_dec_float_100(json::string_t(position["change_today"])),
        .assetMarginable        =                  json::boolean_t(position["asset_marginable"]),
    });
  }

  return result;
}

AlpacaAccountInfo AlpacaApiClient::accountInfo() {
  auto r = this->apiCall(false, "/v2/account", false, {}, {}, {});
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

std::vector<Bar> AlpacaApiClient::bars(std::string symbol,
                            std::chrono::time_point<std::chrono::system_clock> start,
                            std::chrono::time_point<std::chrono::system_clock> end,
                            int64_t limit,
                            std::string timeframe) {
  std::vector<Bar> result;

  std::string nextPageToken = "";
  bool hasNextPage = true;
  while (hasNextPage) {
    // Assemble params
    cpr::Parameters params = {
      { "symbols", symbol },
      { "start", timePointToRfc3339(start) },
      { "end",   timePointToRfc3339(end) },
      { "limit", std::to_string(limit) },
      { "timeframe", timeframe }
    };

    if (nextPageToken != "") params.Add({ "page_token", nextPageToken });

    // Make api call
    auto r = this->apiCall(true, "/v2/stocks/bars", false, {}, {}, params);
    if (r.status_code != 200) throw UnknownAlpacaError();

    // Add bars to output
    json data = json::parse(r.text);
    for (auto bar : data["bars"][symbol]) {
      // TODO: number_float_t passes through double, reducing our precision.
      //       This is incorrect for financial types. We're correcting this with cround()
      //       for now, as double does obviously provide enough precision for hundredths (cents).
      //
      //       Might need to consider a better json library.

      result.push_back({
        // TODO time
        .time   = rfc3339ToTimePoint(json::string_t(bar["t"])),
        .open   = cround(currency(json::number_float_t(bar["o"])), 4),
        .close  = cround(currency(json::number_float_t(bar["c"])), 4),
        .high   = cround(currency(json::number_float_t(bar["h"])), 4),
        .low    = cround(currency(json::number_float_t(bar["l"])), 4),
        .volume = json::number_integer_t(bar["v"])
      });
    }

    hasNextPage = !data["next_page_token"].is_null();
    if (hasNextPage) nextPageToken = json::string_t(data["next_page_token"]);
  }

  return result;
}

std::vector<AlpacaCalendarEntry> AlpacaApiClient::calendar(
    std::chrono::time_point<std::chrono::system_clock> start,
    std::chrono::time_point<std::chrono::system_clock> end) {
  auto r = this->apiCall(false, "/v2/calendar", false, {}, {},
      {{ "start", timePointToRfc3339(start) },
       { "end",   timePointToRfc3339(end)   }});

  if (r.status_code != 200) throw UnknownAlpacaError();

  json data = json::parse(r.text);

  std::vector<AlpacaCalendarEntry> result;

  for (auto entry : data) {
    std::string openStr = json::string_t(entry["date"]) + " " + json::string_t(entry["open"]);
    std::string closeStr = json::string_t(entry["date"]) + " " + json::string_t(entry["close"]);

    result.push_back({
      .open  = toTimePoint(openStr,  "%F %H:%M", "America/New_York"),
      .close = toTimePoint(closeStr, "%F %H:%M", "America/New_York"),
    });
  }

  return result;
}

void AlpacaApiClient::placeOrder(std::string symbol,
                                 int64_t qty, // Positive for long, negative for short
                                 std::string timeInForce,
                                 currency limitPrice, // If less than 0, market order. Else limit order.
                                 bool extendedHours) {

  json postData;

  postData["symbol"] = symbol;

  if (qty > 0) postData["side"] = "buy"; else postData["side"] = "sell";
  if (limitPrice <= 0) postData["type"] = "market"; else postData["type"] = "limit";

  postData["time_in_force"] = timeInForce;
  postData["extended_hours"] = extendedHours;
  postData["qty"] = std::to_string(abs(qty));

  auto r = this->apiCall(false, "/v2/orders", true, postData, {}, {});
  if (r.status_code != 200) throw UnknownAlpacaError();

  // TODO return a client order ID?
  return;
}
