#include "lib/AlpacaApiClient.hpp"
#include <cpr/cpr.h>

using namespace algotrade;

AlpacaApiClient::AlpacaApiClient(std::string key, std::string secret, bool paperMode, FILE* log) : key(key), secret(secret), paperMode(paperMode), log(log) {
  
  if (paperMode) {
    this->baseUrl = "https://paper-api.alpaca.markets";
  } else {
    this->baseUrl = "https://api.alpaca.markets";
  }

  if (this->log) fprintf(this->log, " --- Begin logging for api client --- ");
}

AlpacaAccountInfo AlpacaApiClient::accountInfo() {
  fprintf(stderr, "doing stuff\n");
  cpr::Response r =
    cpr::Get(cpr::Url{this->baseUrl+"/v2/account"},
        cpr::Header{{"APCA-API-KEY-ID", this->key}},
        cpr::Header{{"APCA-API-SECRET-KEY", this->secret}});

  // TODO handle both cpr exceptions and alpaca errors, throwing our own exceptions

  if (this->log) fprintf(this->log, " --- Response:\n %s\n", r.text.c_str());
  return {};
}
