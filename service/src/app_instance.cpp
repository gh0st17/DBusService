#include "app_instance.hpp"

#include <json/json.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

#include "generic/generic.hpp"

namespace fs = std::filesystem;

AppInstance::AppInstance(const fs::path& configPath, const ConnParams& cp) {
  this->configPath = configPath;
  readConfig();

  this->cp = make_unique<ConnParams>(cp);
  const string appName = configPath.stem().string();

  cout << "Creating configuration listener instance for " << appName << endl;
  cout << "Configuration path is '" << configPath.string() << "'" << endl;

  object =
    sdbus::createObject(cp.conn, sdbus::ObjectPath(cp.objectPath + appName));

  object
    ->addVTable(
      sdbus::MethodVTableItem{sdbus::MethodName{"ChangeConfiguration"},
                              sdbus::Signature{"sv"},
                              {},
                              sdbus::Signature{"s"},
                              {},
                              setConfigCallback(),
                              {}},
      sdbus::MethodVTableItem{sdbus::MethodName{"GetConfiguration"},
                              sdbus::Signature{""},
                              {},
                              sdbus::Signature{"a{sv}"},
                              {},
                              getConfigCallback(),
                              {}},
      sdbus::SignalVTableItem{signalName, sdbus::Signature{"sv"}, {}, {}})
    .forInterface(cp.interfaceName);
}

void AppInstance::writeConfig() {
  const lock_guard<mutex> lock(mu);
  ofstream ofs(configPath);
  if (!ofs.is_open()) {
    throw std::runtime_error("can't write file " + configPath.string());
  }

  Json::Value root;

  for (const auto& [key, value] : dict) {
    if (value.containsValueOfType<string>()) {
      root[key] = value.get<string>();
    } else if (value.containsValueOfType<uint>()) {
      root[key] = value.get<uint>();
    } else {
      cerr << "unknown type: " << key << endl;
    }
  }
  ofs << root;

  ofs.close();
}

void AppInstance::readConfig() {
  const lock_guard<mutex> lock(mu);
  generic::readConfig(dict, configPath);
}

sdbus::method_callback AppInstance::getConfigCallback() const {
  return [this](sdbus::MethodCall call) {
    auto reply = call.createReply();
    reply << dict;
    reply.send();
  };
}

sdbus::method_callback AppInstance::setConfigCallback() {
  return [this](sdbus::MethodCall call) {
    string key;
    sdbus::Variant value;

    call >> key >> value;
    auto reply = call.createReply();
    stringstream ss;

    auto handleError = [&](const string& message) {
      cerr << message << endl;
      reply << message;
      reply.send();
    };

    if (dict.find(key) == dict.end()) {
      ss << "unknown key '" << key << "', discarded";
      handleError(ss.str());
      return;
    }

    dict[key] = value;
    try {
      writeConfig();
    } catch (const fs::filesystem_error& e) {
      ss << "filesystem error while writing conf: " << e.what();
      handleError(ss.str());
      return;
    } catch (const Json::Exception& e) {
      ss << "json error while writing conf: " << e.what();
      handleError(ss.str());
      return;
    } catch (const std::exception& e) {
      cerr << "unknown error while writing conf: " << e.what();
      handleError(ss.str());
      return;
    }

    try {
      reply << string{"Key '" + key + "' was set"};
      reply.send();

      auto signal =
        object->createSignal(this->cp->interfaceName, this->cp->signalName);
      signal << key << dict[key];
      object->emitSignal(signal);
    } catch (const sdbus::Error& e) {
      ss << "sdbus error while signaling: " << e.what();
      handleError(ss.str());
    } catch (const std::exception& e) {
      ss << "unknown error while signaling: " << e.what();
      handleError(ss.str());
    }
  };
}