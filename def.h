#pragma once
#include <unordered_set>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QStatusBar>
#include <QtCore/QCoreApplication>

extern unsigned int time_s;
extern std::vector<std::vector<QPointF>> BezierCurveList;
extern float moveCache[360][2];
extern int bullet_player_default_relative;
extern int startingPoint[2];
extern int score, RL; // 残機
extern QStatusBar* MWStatusBar;
extern QGraphicsScene* scene;
class bullet_player_default;
extern std::unordered_set<bullet_player_default*> bpd_instances;

constexpr int sceneWidth(800), sceneHeight(600);
constexpr int sceneWidthHalf(sceneWidth/2), sceneHeightHalf(sceneHeight/2);
constexpr int fpsConstant(16), collidingDetectionConstant(fpsConstant * 5);

template <typename T>
inline void im(T i) {
	QMetaObject::invokeMethod(QCoreApplication::instance(), i);
}


// namespace内の画像ポインタも extern
namespace sp {
	extern QPixmap* A_aqua;
	extern QPixmap* triangle_red;
	extern QPixmap* hexagon_yellow;
	extern QPixmap* triangle_blue;
}

//座標.begin
struct Coord {
	Coord(int i_x, int i_y, int i_t=10, int i_d=0): x(i_x+sceneWidthHalf), y(i_y+sceneHeightHalf), t(i_t), d(i_d) {}
	Coord(double i_x, double i_y, int i_t=10, int i_d=0): x(static_cast<int>(i_x)), y(static_cast<int>(i_y)), t(i_t), d(i_d) {}
	int x, y, d, t;
};
//座標.end

struct CoordSet {
	CoordSet(int time, int i_x, int i_y, int i_t=10, int i_d=0): t(time*10), c(i_x,i_y,i_t,i_d) {}
	int t;
	Coord c;
};
