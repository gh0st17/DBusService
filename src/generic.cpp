#include "generic/generic.hpp"

namespace generic {

  void readConfig(map<string, sdbus::Variant>& dict, const fs::path& configPath) {
    ifstream ifs(configPath);
    if (!ifs.is_open()) {
      throw std::runtime_error("Can't open file: " + configPath.string());
    }

    Json::Value root;
    ifs >> root;

    for (const auto& key : root.getMemberNames()) {
      const Json::Value& val = root[key];

      cout << key << ": " << val.asString() << endl;

      if (val.isUInt()) {
        dict[key] = sdbus::Variant(static_cast<uint>(val.asUInt()));
      } else if (val.isInt()) {
        dict[key] = sdbus::Variant(static_cast<int>(val.asInt()));
      } else if (val.isString()) {
        dict[key] = sdbus::Variant(val.asString());
      } else if (val.isBool()) {
        dict[key] = sdbus::Variant(val.asBool());
      } else {
        cerr << "Unknown type: " << key << std::endl;
      }
    }

    ifs.close();
  }

}