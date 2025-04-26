#include <iostream>
#include <format>
#include <string.h>

using namespace std;

#include "params.hpp"

struct Params p = { nullptr };
const char *usage = "{-p configs_path}";

/// @brief Разбор аргументов программы
/// @param argc Количество аргументов
/// @param argv Массив строк аргументов
void parse_params(const int argc, const char *argv[]) {
  for (size_t i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-p")) {
      if (++i < argc)
        p.configPath = argv[i];
      else
        throw invalid_argument("File path missing");
    }
    else
      cout << format("Unknown argument '{}'\n", argv[i]);
  }

  if (!p.configPath)
    throw invalid_argument(
      format(
        "Input file path empty\n\n{} {}\n", argv[0], usage
      )
    );
}