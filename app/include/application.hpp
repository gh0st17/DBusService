#pragma once
#include <json/json.h>

#include <condition_variable>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>
#include <optional>

#include "generic/logger.hpp"
#include "sdbus-c++/sdbus-c++.h"

using namespace std;
namespace fs = std::filesystem;

/// @brief Псевдоним для типа конфигурации
using config = map<string, sdbus::Variant>;

/// @brief Класс для имитации приложения
class ConfManagerApplication {
 private:
  unique_ptr<sdbus::IConnection> conn_;
  unique_ptr<sdbus::IProxy> proxy_;
  fs::path configPath_;
  Logger& logger_ = Logger::getInstance();
  string appName_;
  config dict_;
  mutex mu_;

  /// @brief Используется для чтения сигнала
  ///        о завершении программы
  condition_variable* cv_;

  const sdbus::ServiceName serviceName_{"com.system.configurationManager"};
  const sdbus::InterfaceName interfaceName_{sdbus::InterfaceName(
    static_cast<string>(serviceName_) + ".Application.Configuration")};
  const sdbus::ObjectPath objectPath_{
    "/com/system/configurationManager/Application/"};
  const sdbus::SignalName signalName_{"configurationChanged"};

  /// @brief Читает файл конфигурации по пути в `configPath_`
  ///        и сохраняет его содержимое в `dict_`
  void readConfig();

  /// @brief Обработчик сигнала
  /// @return Возращает функцию обработки сигнала
  sdbus::signal_handler signalCallback();

 public:
  ConfManagerApplication(const fs::path& configPath, condition_variable* cv);

  /*
   * Удаляем конструкторы копирования
   * Оставляем только конструкторы перемещения
   */

  ConfManagerApplication(const ConfManagerApplication&) = delete;
  ConfManagerApplication(ConfManagerApplication&& other) noexcept
      : conn_(std::move(other.conn_)),
        proxy_(std::move(other.proxy_)),
        configPath_(std::move(other.configPath_)),
        dict_(std::move(other.dict_)),
        serviceName_(other.serviceName_),
        interfaceName_(other.interfaceName_),
        objectPath_(other.objectPath_),
        signalName_(other.signalName_) {
  }

  ConfManagerApplication& operator=(const ConfManagerApplication&) = delete;
  ConfManagerApplication& operator=(ConfManagerApplication&& other) noexcept {
    if (this != &other) {
      conn_ = std::move(other.conn_);
      proxy_ = std::move(other.proxy_);
      configPath_ = std::move(other.configPath_);
      dict_ = std::move(other.dict_);
    }
    return *this;
  }

  /// @brief Запускает прослушивание событии
  void start();

  /// @brief Останавливает прослушивание событии
  void stop();

  /// @brief Ожидает `Timeout`
  /// @param lock Захваченная блокировка
  /// @return `true` если произошло прерывание в течение `Timeout`
  const bool waitTimeout(unique_lock<mutex>& lock) const;

  /// @brief Печатает `TimeoutPhrase`
  void printTimeoutPhrase();
};