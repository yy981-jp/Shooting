#pragma once
#include <cstddef>

#define X(name) struct name {};
#include "components.def"
#undef X
