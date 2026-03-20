#pragma once

#include <vector>
#include <variant>
#include <type_traits>

#include "../core/vec2.h"
#include "../audio/sfx.h"
#include "../core/hitEvent.h"



// ====================
// cmd（生コマンド定義）
// ====================

namespace cmd {

#define GLOBAL(name, body) struct name { body };
#define PLAY(name, body) struct name { body };

#include "../../../shared/cmd.def"

#undef GLOBAL
#undef PLAY

	struct _ {}; // 空用
}



// ====================
// GameCommand（分類済みvariant）
// ====================

namespace GameCommand {

	using Game = std::variant<
#define GLOBAL(name, body) cmd::name,
#define PLAY(name, body)
#include "../../../shared/cmd.def"
#undef GLOBAL
#undef PLAY
		cmd::_
	>;

	using Play = std::variant<
#define GLOBAL(name, body)
#define PLAY(name, body) cmd::name,
#include "../../../shared/cmd.def"
#undef GLOBAL
#undef PLAY
		cmd::_
	>;

}



// ====================
// GCMSStorage（分類ルール）
// ====================

template<class T>
struct GCMSStorage {
	static_assert(sizeof(T) == 0, "GCMSStorage not defined for this cmd");
};

#define GLOBAL(name, body) \
template<> struct GCMSStorage<cmd::name> { \
	using type = std::vector<GameCommand::Game>; \
};

#define PLAY(name, body) \
template<> struct GCMSStorage<cmd::name> { \
	using type = std::vector<GameCommand::Play>; \
};

#include "../../../shared/cmd.def"

#undef GLOBAL
#undef PLAY


template<class T>
using storage_t = typename GCMSStorage<T>::type;


// ====================
// GCMS本体
// ====================

class GCMS {
	std::vector<GameCommand::Game> data_game;
	std::vector<GameCommand::Play> data_play;

	template<class T>
	storage_t<T>& get_storage() {
		using U = std::decay_t<T>;

		if constexpr (std::is_same_v<storage_t<U>, std::vector<GameCommand::Game>>) {
			return data_game;
		} else {
			return data_play;
		}
	}

public:

	// 追加
	template<class T>
	void operator()(T&& c) {
		this->template get_storage<std::decay_t<T>>()
			.emplace_back(std::forward<T>(c));
	}

	// 取得 TODO: 両方該当しない場合の例外処理
	template<class T>
	auto& get() {
		if constexpr (std::is_same_v<T, GameCommand::Game>) {
			return data_game;
		} else {
			return data_play;
		}
	}
};