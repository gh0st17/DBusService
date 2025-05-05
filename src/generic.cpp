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

const bool errorHandler(Func pred, Func fallback) {
  try {
    pred();
    return true;
  } catch (const fs::filesystem_error& e) {
    Logger::getInstance().error() << "filesystem error: " << e.what();
  } catch (const std::bad_cast& e) {
    Logger::getInstance().error() << "bad_cast error: " << e.what();
  } catch (const std::bad_alloc& e) {
    Logger::getInstance().error() << "bad_alloc error: " << e.what();
  } catch (const Json::Exception& e) {
    Logger::getInstance().error() << "json error: " << e.what();
  } catch (const sdbus::Error& e) {
    Logger::getInstance().error() << "sdbus error: " << e.what();
  } catch (const std::invalid_argument& e) {
    Logger::getInstance().error() << "invalid_argument error: " << e.what();
  } catch (const std::exception& e) {
    Logger::getInstance().error() << "unknown error: " << e.what();
  }
  if (fallback != nullptr) {
    generic::errorHandler([&]() {
      fallback();
    });
  }
  return false;
}

atomic<bool> stop = false;

void signalHandler(int signal) {
  if (signal == SIGINT) {
    Logger::getInstance().info() << "Ctrl+C received";
    stop.store(true);
  }
}
}  // namespace generic