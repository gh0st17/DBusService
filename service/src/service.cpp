#include "service.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

DBusService::DBusService(vector<fs::path>&& configsPaths) {
  conn = sdbus::createSessionBusConnection();

  conn->requestName(sdbus::ServiceName(serviceName));

  for (const auto& path : configsPaths) {
    cout << "Read config '" << path.string() << "'" << endl;

    initInstance(path);
  }
}

void DBusService::initInstance(const fs::path& configPath) {
  ConnParams cp{
    .conn = *(this->conn),
    .interfaceName = this->interfaceName,
    .signalName = this->signalName,
    .objectPath = sdbus::ObjectPath(this->objectPath + "Application/")};

  auto instance = make_unique<AppInstance>(configPath, cp);

  instances.push_back(std::move(instance));
}

void DBusService::startService() {
  cout << "Entering into event loop\n";
  conn->enterEventLoop();
}