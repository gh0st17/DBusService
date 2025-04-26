#include <iostream>
#include <format>
#include <string.h>

using namespace std;

#include "params.hpp"

struct Params p;
const char *usage = "{-p configs_path}";

/// @brief Разбор аргументов программы
/// @param argc Количество аргументов
/// @param argv Массив строк аргументов
void parse_params(const int argc, const char *argv[]) {
  for (size_t i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-p")) {
      if (++i < argc)
        p.configsPaths = getJsonFiles(argv[i]);
      else
        throw invalid_argument("File path missing");
    }
    else
      cout << format("Unknown argument '{}'\n", argv[i]);
  }

  if (p.configsPaths.size() == 0) {
    throw invalid_argument(
      format(
        "Input directory doesn't contains any json file\n"
      )
    );
  }
}

vector<fs::path> getJsonFiles(const char* directory) {
  vector<fs::path> jsonFiles;
  for (const auto& entry : fs::directory_iterator(directory)) {
    if (entry.is_regular_file() && entry.path().extension() == ".json") {
      jsonFiles.push_back(entry.path());
    }
  }
  return jsonFiles;
}