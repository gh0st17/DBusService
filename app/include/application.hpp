#pragma once
#include <json/json.h>

#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <optional>

#include "sdbus-c++/sdbus-c++.h"

using namespace std;
namespace fs = std::filesystem;

/// @brief Псевдоним для типа конфигурации
using config = map<string, sdbus::Variant>;

/// @brief Класс для имитации приложения
class ConfManagerApplication {
 private:
  unique_ptr<sdbus::IConnection> conn;
  unique_ptr<sdbus::IProxy> proxy;
  fs::path configPath;
  config dict;

  mutex mu;

  const sdbus::ServiceName serviceName{"com.system.configurationManager"};
  const sdbus::InterfaceName interfaceName{sdbus::InterfaceName(
    static_cast<string>(serviceName) + ".Application.Configuration")};
  const sdbus::ObjectPath objectPath{
    "/com/system/configurationManager/Application/"};
  const sdbus::SignalName signalName{"configurationChanged"};

  /// @brief Читает файл конфигурации по пути в `configPath`
  ///        и сохраняет его содержимое в dict
  void readConfig();

  /// @brief Обработчик сигнала
  /// @return Возращает функцию обработки сигнала
  sdbus::signal_handler signalCallback();

 public:
  ConfManagerApplication(const fs::path& configPath);

  /*
   * Удаляем конструкторы копирования
   * Оставляем только конструкторы перемещения
   */

  ConfManagerApplication(const ConfManagerApplication&) = delete;
  ConfManagerApplication(ConfManagerApplication&& other) noexcept
      : conn(std::move(other.conn)),
        proxy(std::move(other.proxy)),
        configPath(std::move(other.configPath)),
        dict(std::move(other.dict)),
        serviceName(other.serviceName),
        interfaceName(other.interfaceName),
        objectPath(other.objectPath),
        signalName(other.signalName) {
  }

  ConfManagerApplication& operator=(const ConfManagerApplication&) = delete;
  ConfManagerApplication& operator=(ConfManagerApplication&& other) noexcept {
    if (this != &other) {
      conn = std::move(other.conn);
      proxy = std::move(other.proxy);
      configPath = std::move(other.configPath);
      dict = std::move(other.dict);
    }
    return *this;
  }

  /// @brief Запускает прослушивание событии
  void start();

  /// @brief Печатает `TimeoutPhrase`
  void printTimeoutPhrase();

  /// @brief Возвращает значение `Timeout`
  /// @return `Timeout`
  const optional<uint> timeout();

  /// @brief Возвращает имя приложения
  /// @return Имя приложения
  const string appName() const;
};