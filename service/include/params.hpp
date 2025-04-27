#pragma once
#include <filesystem>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

struct Params {
  vector<fs::path> configsPaths;
};

extern struct Params p;

void parse_params(const int argc, const char* argv[]);
vector<fs::path> getJsonFiles(const char* directory);