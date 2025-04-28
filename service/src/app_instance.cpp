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
  configPath_ = configPath;
  readConfig();

  cp_ = make_unique<ConnParams>(cp);
  const string appName = configPath.stem().string();

  cout << "Creating configuration listener instance for " << appName << endl;
  cout << "Configuration path is '" << configPath.string() << "'" << endl;

  object_ =
    sdbus::createObject(cp.conn, sdbus::ObjectPath(cp.objectPath + appName));

  object_
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
      sdbus::SignalVTableItem{signalName_, sdbus::Signature{"sv"}, {}, {}})
    .forInterface(cp.interfaceName);
}

void AppInstance::writeConfig() {
  const lock_guard<mutex> lock(mu_);
  ofstream ofs(configPath_);
  if (!ofs.is_open()) {
    throw std::runtime_error("can't write file " + configPath_.string());
  }

  Json::Value root;

  for (const auto& [key, value] : dict_) {
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
  const lock_guard<mutex> lock(mu_);
  generic::readConfig(dict_, configPath_);
}

sdbus::method_callback AppInstance::getConfigCallback() const {
  return [this](sdbus::MethodCall call) {
    auto reply = call.createReply();
    reply << dict_;
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

    if (dict_.find(key) == dict_.end()) {
      ss << "unknown key '" << key << "', discarded";
      handleError(ss.str());
      return;
    }

    if (!strcmp(dict_[key].peekValueType(), value.peekValueType())) {
      dict_[key] = value;
    } else {
      ss << "invalid type of key '" << key << "', expected '"
         << dict_[key].peekValueType() << "'";
      handleError(ss.str());
      return;
    }

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

      auto signal = object_->createSignal(cp_->interfaceName, cp_->signalName);
      signal << key << dict_[key];
      object_->emitSignal(signal);
    } catch (const sdbus::Error& e) {
      ss << "sdbus error while signaling: " << e.what();
      handleError(ss.str());
    } catch (const std::exception& e) {
      ss << "unknown error while signaling: " << e.what();
      handleError(ss.str());
    }
  };
}