#include <iostream>
#include <format>
#include <string.h>

using namespace std;

#include "app_params.hpp"

struct AppParams p;
const char *usage = "{-p config_path}";

/// @brief Разбор аргументов программы
/// @param argc Количество аргументов
/// @param argv Массив строк аргументов
void parse_params(const int argc, const char *argv[]) {
  for (size_t i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-p")) {
      if (++i < argc)
        p.configPath = fs::path(argv[i]);
      else
        throw invalid_argument("File path missing");
    }
    else
      cout << format("Unknown argument '{}'\n", argv[i]);
  }
}