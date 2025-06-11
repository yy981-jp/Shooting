	/* ---STD--- */
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <thread>
#include <functional>
#include <fstream>
#include <unordered_set>
// #include <mutex>

	/* ---OTHER--- */
#include <nlohmann/json.hpp>
#include <boost/signals2/signal.hpp>
#include <yy981/debug.h>
#include <yy981/thread.h>

	/* ---WINDOWS--- */
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

	/* ---QT--- */
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>
#include <QtCore/QPointF>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLabel>
#include <QtGui/QKeyEvent>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtGui/QPainterPath>
#include <QtGui/QSurfaceFormat>
#include <QtOpenGLWidgets/QOpenGLWidget>


#define BML [](int L, QGraphicsPixmapItem* I)
#define BMLN [L,I]()


unsigned int time_s = 0;
constexpr int sceneWidth = 800;
constexpr int sceneHeight = 600;
constexpr int sceneWidthHalf = sceneWidth/2;
constexpr int sceneHeightHalf = sceneHeight/2;
// 60 -> 16, 30 -> 33
constexpr int fpsConstant = 16;
constexpr int collidingDetectionConstant = fpsConstant*5;
std::vector<std::vector<QPointF>> BezierCurveList;
float moveCache[360][2];
int bullet_player_default_relative;
int startingPoint[2];
int score = 0;
int RL = 100; // 残機
QStatusBar* MWStatusBar;
QGraphicsScene* scene;
class bullet_player_default;
std::unordered_set<bullet_player_default*> bpd_instances;


#ifdef DEBUG
	void DBG(const std::string str) {std::cout << "DEBUG::: " << str << "\n";}
#else
	void DBG(const std::string str) {}
#endif

template <typename T>
inline void im(T i) {
	QMetaObject::invokeMethod(QCoreApplication::instance(), i);
}
/*
int subnum(int input, int start, int length, bool back = false) {
	if (back) {
		int divisorStart = static_cast<int>(pow(10, start));
		if (length==0) return (input/divisorStart);
			else return (input/divisorStart) % static_cast<int>(pow(10, length));
	} else {
		int totalDigits = static_cast<int>(log10(input)) + 1;
		if (length==0) return (input % static_cast<int>(pow(10, totalDigits - start)));
			else return (input % static_cast<int>(pow(10, totalDigits - start))) / static_cast<int>(pow(10, totalDigits - start - length));
	}
}
*/
namespace sp {
	QPixmap* A_aqua;
	QPixmap* triangle_red;
	QPixmap* hexagon_yellow;
	QPixmap* triangle_blue;
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
