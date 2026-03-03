#include "cache.h"

#include <cmath>

CacheSV::CacheSV() {
	for (int i = 0; i < TABLE_SIZE; ++i) {
		float rad = TWO_PI * i / TABLE_SIZE;
		table[i][0] = std::cos(rad);
		table[i][1] = std::sin(rad);
	}
}

vec2f CacheSV::getDir(float rad) const {
	int idx = toIndex(rad);
	return { table[idx][0], table[idx][1] };
}

float CacheSV::getSin(float rad) const {
	int idx = toIndex(rad);
	return table[idx][1];
}

float CacheSV::getCos(float rad) const {
	int idx = toIndex(rad);
	return table[idx][0];
}

int CacheSV::toIndex(float rad) {
	int idx = static_cast<int>(rad * INV_STEP);
	return (idx - OFFSET_INDEX) & TABLE_MASK;
}


CacheSV cachesv;