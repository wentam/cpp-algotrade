#pragma once

#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace boost::multiprecision;

namespace algotrade {
  typedef cpp_dec_float_100 currency;
  typedef int64_t unix_time_seconds;

  struct Bar {
    unix_time_seconds time;
    currency open;
    currency close;
    currency high;
    currency low;
    int64_t  volume;
  };
}

