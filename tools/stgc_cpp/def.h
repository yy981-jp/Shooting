#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <charconv>

constexpr int MAX_NEST = 32;

using Value = std::variant<bool, int, double, std::string>;
using ArgMap = std::unordered_map<std::string, Value>;
