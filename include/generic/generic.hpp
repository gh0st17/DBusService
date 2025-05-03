#pragma once
#include <json/json.h>

#include <atomic>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <map>

#include "sdbus-c++/sdbus-c++.h"

using namespace std;
namespace fs = std::filesystem;

/// @brief Пространство имен для общих функции проекта
namespace generic {

/// @brief Читает конфигурацию из файла по пути
///        `configPath` в `dict`
/// @param dict Словарь с ключами конфигурации
/// @param configPath Путь до файла с конфигурацией
void readConfig(map<string, sdbus::Variant>& dict, const fs::path& configPath);

/// @brief Преобразует значение `sdbus::Variant` конфигурации
///        в строку в рамках заведомо известного набора типов
/// @param value Значение типа `sdbus::Variant`
/// @return Значение в строковом представлении
const string stringValue(const sdbus::Variant& value);

/// @brief Печатает конфигурацию приложения
/// @param dict Словарь с ключами конфигурации
/// @param appName Имя приложения для отображения
void printConfig(const map<string, sdbus::Variant>& dict,
                 const string& appName);

/// @brief Флаг остановки программы
extern atomic<bool> stop;

/// @brief Обработчик сигналов
/// @param signal Код сигнала
void signalHandler(int signal);
}  // namespace generic