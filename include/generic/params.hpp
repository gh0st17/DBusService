#pragma once
#include <filesystem>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

/// @brief Структура для хранения
///        входных параметров программы
struct Params {
  vector<fs::path> configsPaths;
};

extern struct Params p;

/// @brief Разбор аргументов программы
/// @param argc Количество аргументов
/// @param argv Массив строк аргументов
void parse_params(const int argc, const char* argv[]);

/// @brief Собирает `jscon` файлы в директории
/// @param directory Директория для поиска `json` файлов
/// @return Массив путей с файлами `json`
vector<fs::path> getJsonFiles(const char* directory);