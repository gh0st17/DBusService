#include "generic/generic.hpp"

#include "generic/logger.hpp"

namespace generic {

void readConfig(map<string, sdbus::Variant>& dict, const fs::path& configPath) {
  ifstream ifs(configPath);
  if (!ifs.is_open()) {
    throw std::runtime_error("can't open file: " + configPath.string());
  }

  Json::Value root;
  ifs >> root;

  for (const auto& key : root.getMemberNames()) {
    const Json::Value& val = root[key];

    if (val.isString()) {
      dict[key] = sdbus::Variant(val.asString());
    } else if (val.isUInt()) {
      dict[key] = sdbus::Variant(static_cast<uint>(val.asUInt()));
    } else {
      Logger::getInstance().error() << "readConfig: unknown type: " + key;
    }
  }

  ifs.close();
}

const string stringValue(const sdbus::Variant& value) {
  if (value.containsValueOfType<uint>()) {
    return to_string(value.get<uint>());
  } else if (value.containsValueOfType<string>()) {
    return "'" + value.get<string>() + "'";
  } else {
    return "unknown value type";
  }
}

void printConfig(const map<string, sdbus::Variant>& dict,
                 const string& appName) {
  if (dict.size() > 0) {
    auto info = Logger::getInstance().info();
    info << appName + ": config: \n";

    for (const auto& [key, value] : dict) {
      info << key + ": " + generic::stringValue(value) << endl;
    }
  } else {
    throw std::runtime_error(appName + ": empty configuration file");
  }
}

atomic<bool> stop = false;

void signalHandler(int signal) {
  if (signal == SIGINT) {
    Logger::getInstance().info() << "Ctrl+C received";
    stop.store(true);
  }
}
}  // namespace generic