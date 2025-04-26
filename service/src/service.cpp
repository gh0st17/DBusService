#include <iostream>
#include <fstream>
#include <filesystem>
#include "service.hpp"

namespace fs = std::filesystem;

DBusService::DBusService(vector<fs::path>&& configsPaths) {
  conn = sdbus::createSessionBusConnection();

  conn->requestName(sdbus::ServiceName(serviceName));

  for (const auto& path : configsPaths) {
    cout << format("Read config '{}'\n", path.string());

    initInstance(path);
  }
}

void DBusService::initInstance(const fs::path& configPath) {
  ConnParams cp{
    .conn          = *(this->conn),
    .interfaceName = this->interfaceName,
    .objectPath    = this->objectPath
  };

  auto instance = make_unique<AppInstance>(configPath, cp);

  instances.push_back(std::move(instance));
}

void DBusService::startService() {
  cout << "Entering into event loop\n";
  conn->enterEventLoop();
}