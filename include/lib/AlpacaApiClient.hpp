#include <string>

namespace algotrade {
  struct AlpacaAccountInfo {

  };

  class AlpacaApiClient {
    public:
      // log can be NULL if you don't want logging
      AlpacaApiClient(std::string key, std::string secret, bool paperMode, FILE* log);

      AlpacaAccountInfo accountInfo();

    private:
      std::string key;
      std::string secret;
      bool paperMode;
      FILE* log;
      std::string baseUrl;
  };
}
