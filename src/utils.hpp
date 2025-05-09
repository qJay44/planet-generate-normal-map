#pragma once

#include <format>
#include <string>

#include "clrp.hpp"

std::string readFile(const fspath& path);

void printTabs(u8 n);
void error(const std::string& msg);
void warning(const std::string& msg);

inline void print(const vec3& v, const std::string& name = "vec") {
  printf("%s\n", std::format("{}: {}, {}, {}", name, v.x, v.y, v.z).c_str());
}

