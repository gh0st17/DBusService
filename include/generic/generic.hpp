#pragma once
#include <json/json.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

#include "sdbus-c++/sdbus-c++.h"

using namespace std;
namespace fs = std::filesystem;

/// @brief Пространство имен для общих функции проекта
namespace generic {

/// @brief Читает конфигурацию из файла по пути
///        `configPath` в `dict`
/// @param dict
/// @param configPath
void readConfig(map<string, sdbus::Variant>& dict, const fs::path& configPath);
}  // namespace generic