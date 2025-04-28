#include "application.hpp"

#include <fstream>
#include <iostream>

#include "generic/generic.hpp"

ConfManagerApplication::ConfManagerApplication(const fs::path& configPath) {
  this->configPath = configPath;
  readConfig();
  const string appName = configPath.stem().string();

  conn = sdbus::createSessionBusConnection();

  proxy = sdbus::createProxy(*conn, serviceName,
                             sdbus::ObjectPath(objectPath + appName));
  proxy->registerSignalHandler(interfaceName, signalName, signalCallback());
}

sdbus::signal_handler ConfManagerApplication::signalCallback() {
  sdbus::signal_handler sh = [this](sdbus::Signal signal) {
    string key;
    sdbus::Variant value;
    signal >> key >> value;

    cout << appName() << ": recieved key: " << key << endl;

    const lock_guard<mutex> lock(mu);
    if (dict.find(key) == dict.end()) {
      cerr << "unknown key '" << key << "', discarded" << endl;
      return;
    }

    dict[key] = value;
  };

  return sh;
}

void ConfManagerApplication::readConfig() {
  const lock_guard<mutex> lock(mu);
  generic::readConfig(dict, configPath);
}

void ConfManagerApplication::start() {
  conn->enterEventLoopAsync();
}

void ConfManagerApplication::printTimeoutPhrase() {
  const lock_guard<mutex> lock(mu);

  if (dict.find("TimeoutPhrase") == dict.end()) {
    cout << appName() << ": TimeoutPhrase: <key unset>\n";
  } else if (dict["TimeoutPhrase"].containsValueOfType<string>()) {
    string value = dict["TimeoutPhrase"].get<string>();
    cout << appName() << ": TimeoutPhrase: '" << value << "'" << endl;
  } else {
    cout << appName() << ": TimeoutPhrase is not string type\n";
  }
}

const optional<uint> ConfManagerApplication::timeout() {
  const lock_guard<mutex> lock(mu);

  if (dict.find("Timeout") == dict.end()) {
    cout << appName() << ": <Timeout unset>\n";
  } else if (dict["Timeout"].containsValueOfType<uint>()) {
    return dict["Timeout"].get<uint>();
  } else {
    cout << appName() << ": Timeout is not uint type\n";
  }

  return {};
}

const string ConfManagerApplication::appName() const {
  return configPath.stem().string();
}