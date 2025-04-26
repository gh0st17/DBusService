#pragma once

struct Params {
  const char* configPath;
};

extern struct Params p;

void parse_params(const int argc, const char *argv[]);