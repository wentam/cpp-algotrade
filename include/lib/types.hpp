#pragma once

#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace boost::multiprecision;

namespace algotrade {
  typedef cpp_dec_float_100 currency;

  struct Bar {
    std::chrono::time_point<std::chrono::system_clock> time;
    currency open;
    currency close;
    currency high;
    currency low;
    int64_t  volume;
  };

}

