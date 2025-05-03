#include "application.hpp"

#include <fstream>

#include "generic/generic.hpp"

ConfManagerApplication::ConfManagerApplication(const fs::path& configPath)
    : configPath_(configPath), appName_(configPath.stem().string()) {
  readConfig();
  generic::printConfig(dict_, appName_);

  conn_ = sdbus::createSessionBusConnection();
  proxy_ = sdbus::createProxy(*conn_, serviceName_,
                              sdbus::ObjectPath(objectPath_ + appName_));
  proxy_->registerSignalHandler(interfaceName_, signalName_, signalCallback());
}

sdbus::signal_handler ConfManagerApplication::signalCallback() {
  return [this](sdbus::Signal signal) {
    thread([this, signal = std::move(signal)]() mutable {
      string key;
      sdbus::Variant value;
      signal >> key >> value;

      logger_.info() << appName_ + ": recieved key: " + key;

      const lock_guard<mutex> lock(mu_);
      if (dict_.find(key) == dict_.end()) {
        logger_.error() << "unknown key '" + key + "', discarded";
        return;
      }

      dict_[key] = value;
    }).detach();
  };
}

void ConfManagerApplication::readConfig() {
  const lock_guard<mutex> lock(mu_);
  generic::readConfig(dict_, configPath_);
}

void ConfManagerApplication::start() {
  conn_->enterEventLoopAsync();
}

void ConfManagerApplication::printTimeoutPhrase() {
  const lock_guard<mutex> lock(mu_);

  if (dict_.find("TimeoutPhrase") == dict_.end()) {
    logger_.info() << appName_ + ": TimeoutPhrase: <key unset>";
  } else if (dict_["TimeoutPhrase"].containsValueOfType<string>()) {
    string value = dict_["TimeoutPhrase"].get<string>();
    logger_.info() << appName_ + ": TimeoutPhrase: '" + value + "'";
  } else {
    logger_.warning() << appName_ + ": TimeoutPhrase is not string type";
  }
}

const optional<uint> ConfManagerApplication::timeout() {
  const lock_guard<mutex> lock(mu_);

  if (dict_.find("Timeout") == dict_.end()) {
    logger_.info() << appName_ + ": <Timeout unset>";
  } else if (dict_["Timeout"].containsValueOfType<uint>()) {
    return dict_["Timeout"].get<uint>();
  } else {
    logger_.warning() << appName_ + ": Timeout is not uint type";
  }

  return {};
}