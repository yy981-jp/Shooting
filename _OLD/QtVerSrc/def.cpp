#include "def.h"


unsigned int time_s = 0;
std::vector<std::vector<QPointF>> BezierCurveList;
float moveCache[360][2] = {};
int bullet_player_default_relative = 0;
int startingPoint[2] = {0, 0};
int score = 0, RL = 404;
QStatusBar* MWStatusBar = nullptr;
QGraphicsScene* scene = nullptr;
std::unordered_set<bullet_player_default*> bpd_instances;

namespace sp {
	QPixmap* A_aqua = nullptr;
	QPixmap* triangle_red = nullptr;
	QPixmap* hexagon_yellow = nullptr;
	QPixmap* triangle_blue = nullptr;
}
