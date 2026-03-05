#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>


namespace stg {
	struct Wait { int32_t ms; };
	struct WaitUntil { std::string condition; };

	struct Spawn {
		std::string entityType;
		std::unordered_map<std::string, std::variant<int64_t, double, bool, std::string>> args;
	};

	struct Call { int32_t address; };
	struct Repeat { int32_t count; };
	struct RepeatUntil { std::string condition; };
	struct End {};
	struct Shutdown {};

	using Instruction = std::variant<
		Wait,
		WaitUntil,
		Spawn,
		Call,
		Repeat,
		RepeatUntil,
		End,
		Shutdown
	>;

	#pragma pack(push, 1)
	struct CmdHeader {
		uint16_t size;
		uint16_t type;
	};
	#pragma pack(pop)

	#define X(name, body) \
		struct Spawn_##name { body };

	#include "../../common/spawn.def"
	#undef X
}
