#pragma once
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

struct AppParams {
  fs::path configPath;
};

extern struct AppParams p;

void parse_params(const int argc, const char *argv[]);