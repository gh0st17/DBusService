#include "application.hpp"

#include <fstream>
#include <iostream>

#include "generic/generic.hpp"

ConfManagerApplication::ConfManagerApplication(const fs::path& configPath)
    : configPath_(configPath) {
  readConfig();

  const string appName = configPath_.stem().string();
  conn_ = sdbus::createSessionBusConnection();
  proxy_ = sdbus::createProxy(*conn_, serviceName_,
                              sdbus::ObjectPath(objectPath_ + appName));
  proxy_->registerSignalHandler(interfaceName_, signalName_, signalCallback());
}

sdbus::signal_handler ConfManagerApplication::signalCallback() {
  sdbus::signal_handler sh = [this](sdbus::Signal signal) {
    thread([this, signal = std::move(signal)]() mutable {
      string key;
      sdbus::Variant value;
      signal >> key >> value;

      cout << appName() << ": recieved key: " << key << endl;

      const lock_guard<mutex> lock(mu_);
      if (dict_.find(key) == dict_.end()) {
        cerr << "unknown key '" << key << "', discarded" << endl;
        return;
      }

      dict_[key] = value;
    }).detach();
  };

  return sh;
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
    cout << appName() << ": TimeoutPhrase: <key unset>\n";
  } else if (dict_["TimeoutPhrase"].containsValueOfType<string>()) {
    string value = dict_["TimeoutPhrase"].get<string>();
    cout << appName() << ": TimeoutPhrase: '" << value << "'" << endl;
  } else {
    cout << appName() << ": TimeoutPhrase is not string type\n";
  }
}

const optional<uint> ConfManagerApplication::timeout() {
  const lock_guard<mutex> lock(mu_);

  if (dict_.find("Timeout") == dict_.end()) {
    cout << appName() << ": <Timeout unset>\n";
  } else if (dict_["Timeout"].containsValueOfType<uint>()) {
    return dict_["Timeout"].get<uint>();
  } else {
    cout << appName() << ": Timeout is not uint type\n";
  }

  return {};
}

const string ConfManagerApplication::appName() const {
  return configPath_.stem().string();
}