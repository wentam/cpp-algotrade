#pragma once

#include "lib/types.hpp"
#include <chrono>

using namespace std::chrono_literals;


namespace algotrade {
  [[maybe_unused]]
  static std::string timePointToRfc3339(std::chrono::time_point<std::chrono::system_clock> timePoint) {
     auto timeT = std::chrono::system_clock::to_time_t(timePoint);
     struct tm* tm = gmtime(&timeT);
     char out[256];
     strftime(out, 256,  "%Y-%m-%dT%H:%M:%SZ", tm);
     return out;
  }

  // TODO: this isn't fully RFC3339 compliant
  // * we don't support fractions of a second
  // * dates containing a leap second results is a 0 time point
  [[maybe_unused]]
  static std::chrono::time_point<std::chrono::system_clock> rfc3339ToTimePoint(std::string rfc3339) {
    using namespace std::chrono;
    if (rfc3339.back() == 'Z') {
      std::istringstream in{rfc3339};
      sys_seconds t;
      in >> parse("%F T %H : %M : %S Z", t);
      return t;
    } else {
      std::istringstream in{rfc3339};
      sys_seconds t;
      in >> parse("%F T %H : %M : %S %Ez", t);
      return t;
    }
  }

  [[maybe_unused]]
  static double dround( double f, int places ) {
      double n = std::pow(10.0, places);
      return std::round(f*n)/n;
  }

  [[maybe_unused]]
  static currency cround(currency f, int places) {
    currency n = pow(currency(10.0), places);
    return round(f*n)/n;
  }
}
