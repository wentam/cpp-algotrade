#include "lib/AlpacaApiClient.hpp"
#include "lib/util.hpp"
#include <yaml-cpp/yaml.h>

std::string secretcmd(std::string cmd) {
  FILE* pipe = popen(cmd.c_str(), "r");
  char buffer[128];
  std::string result;
  if (!pipe) throw std::runtime_error("popen() failed!");
  try {
    while (fgets(buffer, sizeof buffer, pipe) != NULL) { result += buffer; }
  } catch (...) {
    pclose(pipe);
    throw;
  }
  pclose(pipe);

  std::string cleanStr;
  for (auto c : result) {
    if (c != ' ' && c != '\n') cleanStr.push_back(c);
  }

  return cleanStr;
}

int main() {
  // Read config file
  // TODO command line argument to specify config file?
  auto yamlConfig = YAML::LoadFile("config.yaml");

  // Validate config
  if (!yamlConfig["alpaca-paper-api-key-cmd"]) {
    printf("Error: alpaca-paper-api-key-cmd config option not found. \n");
    return 1;
  }
  if (!yamlConfig["alpaca-paper-api-secret-cmd"]) {
    printf("alpaca-paper-api-secret-cmd config option not found\n");
    return 1;
  }
  if (!yamlConfig["alpaca-real-api-key-cmd"]) {
    printf("alpaca-real-api-key-cmd config option not found\n");
    return 1;
  }
  if (!yamlConfig["alpaca-real-api-secret-cmd"]) {
    printf("alpaca-real-api-secret-cmd option not found \n");
    return 1;
  }
  if (!yamlConfig["state-path"]) {
    printf("state-path option not found \n");
    return 1;
  }

  // Pull config values
  std::string alpacaPaperApiKey =
    secretcmd(yamlConfig["alpaca-paper-api-key-cmd"].as<std::string>());
  std::string alpacaPaperApiSecret =
    secretcmd(yamlConfig["alpaca-paper-api-secret-cmd"].as<std::string>());
  std::string alpacaRealApiKey =
    secretcmd(yamlConfig["alpaca-real-api-key-cmd"].as<std::string>());
  std::string alpacaRealApiSecret =
    secretcmd(yamlConfig["alpaca-real-api-secret-cmd"].as<std::string>());
  std::string statePath = yamlConfig["state-path"].as<std::string>();

  algotrade::AlpacaApiClient alpaca(alpacaPaperApiKey, alpacaPaperApiSecret, true, 200, stderr);

  //algotrade::AlpacaClock clock = alpaca.clock();

  //algotrade::AlpacaAccountInfo info = alpaca.accountInfo();

  //fprintf(stderr, "%s\n", info.buyingPower.str(100).c_str());

  //auto bars = alpaca.bars("SPY",
  //                        std::chrono::system_clock::now()-10h,
  //                        std::chrono::system_clock::now(),
  //                        9000,
  //                        "1Min");

  //for (auto bar : bars) {
  //  fprintf(stderr, "%ld\n", std::chrono::duration_cast<std::chrono::seconds>(bar.time.time_since_epoch()).count());
  //}

  //fprintf(stderr, "%ld\n", sizeof(std::chrono::time_point<std::chrono::system_clock>));

  //auto result = alpaca.calendar(std::chrono::system_clock::now()-48h,
  //                              std::chrono::system_clock::now());

  //for (auto entry : result) {
  //  fprintf(stderr, "%ld %ld\n",
  //      std::chrono::duration_cast<std::chrono::seconds>(entry.open.time_since_epoch()).count(),
  //      std::chrono::duration_cast<std::chrono::seconds>(entry.close.time_since_epoch()).count());
  //}

  //auto asset = alpaca.asset("SPY");
  //fprintf(stderr, "%s\n", asset.status.c_str());

  //auto positions = alpaca.positions();
  //for (auto position : positions) fprintf(stderr, "%s\n", position.symbol.c_str());

  //auto assets = alpaca.assets();
  //for (auto asset : assets) fprintf(stderr, "%s\n", asset.symbol.c_str());

  //alpaca.placeOrder("SPY", 1, "day", -1, false);

  return 0;
}
