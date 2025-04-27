#pragma once
#include <filesystem>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

struct AppParams {
  vector<fs::path> configsPaths;
};

extern struct AppParams p;

void parse_params(const int argc, const char* argv[]);
vector<fs::path> getJsonFiles(const char* directory);