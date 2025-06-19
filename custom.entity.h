#pragma once
#include "basic.entity.h"


class bullet_arrow: public bullet {
public:
	bullet_arrow(const Coord& coord);
};



class enemy_default: public enemy {
public:
	enemy_default(const Coord& i_coord);
private:
	void process() override;
	Coord coord;
};
