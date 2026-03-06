#pragma once
#include <cstdint>

enum class STGEntityID : uint16_t {

#define ENTITY(name, fields) name,
#define FIELD(...)
#include "stgEntity.xmac"
#undef ENTITY
#undef FIELD

Count
};

namespace STGEntity {

#pragma pack(1)

using s8 = int8_t;
// using u8 = uint8_t;
using s16 = int16_t;
// using u16 = uint16_t;
using s32 = int32_t;
// using u32 = uint32_t;
using f = float;

// unsignedは許容されない

#define ENTITY(name, fields) \
struct name { \
    fields \
};

#define FIELD_CHOOSER(_1,_2,_3,NAME,...) NAME
#define FIELD(...) FIELD_CHOOSER(__VA_ARGS__, FIELD3, FIELD2)(__VA_ARGS__)

#define FIELD2(type, name) \
    type name;

#define FIELD3(type, name, default_value) \
    type name = default_value;

#include "stgEntity.xmac"

#undef FIELD
#undef FIELD2
#undef FIELD3
#undef FIELD_CHOOSER
#undef ENTITY

#pragma pack()

}
