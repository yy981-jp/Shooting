#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

#include "EntityTable.h"
#include "ParamTable.h"
#include "vec2.h"

using entityID = uint16_t;
extern EntityTable entityTable;
extern ParamTable paramTable;

constexpr std::string Assets = "../../assets/";
