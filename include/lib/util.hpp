#pragma once

#include "lib/types.hpp"
#include <chrono>

using namespace std::chrono_literals;


namespace algotrade {
  [[maybe_unused]] static
  std::string timePointToRfc3339(std::chrono::time_point<std::chrono::system_clock> timePoint) {
     auto timeT = std::chrono::system_clock::to_time_t(timePoint);
     struct tm* tm = gmtime(&timeT);
     char out[256];
     strftime(out, 256,  "%Y-%m-%dT%H:%M:%SZ", tm);
     return out;
  }

  [[maybe_unused]]
  static std::string unixTimeToRfc3339(long unixTime) {
    struct tm* tm = gmtime(&unixTime);
    char out[256];
    strftime(out, 256,  "%Y-%m-%dT%H:%M:%SZ", tm);
    return out;
  }

  // TODO: this isn't fully RFC3339 compliant
  // * we don't support fractions of a second
  // * dates containing a leap second results is a 0 time point
  [[maybe_unused]] static
  std::chrono::time_point<std::chrono::system_clock> rfc3339ToTimePoint(std::string rfc3339) {
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
  static unix_time_seconds rfc3339ToUnixTime(std::string rfc3339) {
    using namespace std::chrono;
    return duration_cast<seconds>(rfc3339ToTimePoint(rfc3339).time_since_epoch()).count();
  }

  [[maybe_unused]]
  static std::chrono::time_point<std::chrono::system_clock>
  toTimePoint(std::string ymdhm, std::string format, std::string fromTimeZone) {
    using namespace std::chrono;

    const std::chrono::time_zone* tz = std::chrono::locate_zone(fromTimeZone);

    std::istringstream in{ymdhm};

    std::chrono::time_point<std::chrono::local_t, std::chrono::seconds> t ;
    in >> parse(format, t);

    return tz->to_sys(t);
  }

  [[maybe_unused]]
  static unix_time_seconds toUnixTime(std::string string, std::string format, std::string fromTimeZone) {
    using namespace std::chrono;
    return
      duration_cast<seconds>(
        toTimePoint(string, format, fromTimeZone).time_since_epoch()
      ).count();
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
