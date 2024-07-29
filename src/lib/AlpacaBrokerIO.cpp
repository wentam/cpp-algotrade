#include "lib/BrokerIO.hpp"

using namespace algotrade;

AlpacaBrokerIO::AlpacaBrokerIO(AlpacaApiClient* alpaca) : alpaca(alpaca) {}

BrokerIO::AccountInfo AlpacaBrokerIO::accountInfo() {
  AlpacaAccountInfo info = this->alpaca->accountInfo();

  return {
    .balance            = info.cash,
    .equity             = info.equity,
    .canShort           = info.shortingEnabled
  };
}

void AlpacaBrokerIO::placeOrder(std::string asset,
                                int64_t qty,
                                currency limitPrice,
                                TIF tif,
                                bool extendedHours) {
  if (qty > 0) fprintf(stderr, "+");
  fprintf(stderr, "%ld %s\n", asset.c_str());

  std::string tifStr = "";
  if (tif == TIF::DAY) {
    tifStr = "day";
  } else if (tif == TIF::GOOD_TILL_CANCELLED) {
    tifStr = "gtc";
  }

  this->alpaca->placeOrder(asset, qty, tifStr, limitPrice, extendedHours);
}
