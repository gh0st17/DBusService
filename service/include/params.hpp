#pragma once
#include <filesystem>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

struct Params {
  vector<string> configsPaths;
};

extern struct Params p;

void parse_params(const int argc, const char *argv[]);
vector<string> getJsonFiles(const char* directory);