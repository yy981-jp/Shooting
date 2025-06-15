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

#include "BezierMover.h"
#include "def.h"
#include "entity.basic.h"


#define BML [](int L, QGraphicsPixmapItem* I)
#define BMLN [L,I]()




#ifdef DEBUG
	void DBG(const std::string str) {std::cout << "DEBUG::: " << str << "\n";}
#else
	void DBG(const std::string str) {}
#endif

template <typename T>
inline void im(T i) {
	QMetaObject::invokeMethod(QCoreApplication::instance(), i);
}



// --------------------------------------------------------------------------------------------------------------------------------------------------
			/* custom object */
class bullet_arrow: public bullet {
public:
	bullet_arrow(const Coord& coord): bullet(coord) {setPixmap(*sp::A_aqua);}
};



class enemy_default: public enemy {
public:
	enemy_default(const Coord& i_coord): coord(i_coord), enemy(i_coord) {process();}
private:
	void process() override {
		setPixmap(*sp::triangle_red);
		std::vector<QPointF> controlPoints = BezierCurveList[coord.d];
		for (QPointF& current : controlPoints) {
			current.setX(current.x()+x());
			current.setY(current.y()+y());
		}
		for (QPointF& current : controlPoints) controlPoints.insert(controlPoints.begin(),QPointF(x(),y()));
			// std::cout << "[ct]: " << coord.t << " [02]: " << subnum(coord.t,0,2,true) << " [20]: " << subnum(coord.t,2,0,true) << "\n";

		new BezierMover(this, controlPoints, BML {
/*			if (L%30==10) for (int i = -1; i <= 1; i++) {
				new bullet_arrow(Coord(I->x()+i,I->y(),10,i*7));
			};
*/			if (L%15==0) new bullet_arrow(Coord(I->x(),I->y()));
		},coord.t*1000);
	}
	
	Coord coord;
};






// --------------------------------------------------------------------------------------------------------------------------------------------------
			/* Player */


//Player.begin
class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Player(): leftPressed(false), rightPressed(false), upPressed(false), downPressed(false), distance(5), firing(false) {
        setPixmap(QPixmap("image/triangle_blue.png"));
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Player::updatePosition);
        timer->start(fpsConstant); // 約60fpsで更新
		std::thread([this]{
			while(true) {
				sleepc(tu::l,100);
				if (firing) im([this] {
					new bullet_player_default(Coord(x()+bullet_player_default_relative+10, y()-10,1));
				});
			}
		}).detach();
    }

protected:
    void keyPressEvent(QKeyEvent* event) override {
        switch (event->key()) {
            case Qt::Key_Left: leftPressed = true; break;
            case Qt::Key_Right: rightPressed = true; break;
            case Qt::Key_Up: upPressed = true; break;
            case Qt::Key_Down: downPressed = true; break;
            case Qt::Key_Shift: distance = 2; break;
            case Qt::Key_Z: firing = true; break;
        }
    }

    void keyReleaseEvent(QKeyEvent* event) override {
        switch (event->key()) {
            case Qt::Key_Left: leftPressed = false; break;
            case Qt::Key_Right: rightPressed = false; break;
            case Qt::Key_Up: upPressed = false; break;
            case Qt::Key_Down: downPressed = false; break;
            case Qt::Key_Shift: distance = 5; break;
            case Qt::Key_Z: firing = false; break;
        }
    }
	
	void focusOutEvent(QFocusEvent *event) override {
        QGraphicsPixmapItem::focusOutEvent(event);
		setFocus();
    }

private slots:
    void updatePosition() {
        float dx = 0;
        float dy = 0;

        if (leftPressed) dx -= distance;
        if (rightPressed) dx += distance;
        if (upPressed) dy -= distance;
        if (downPressed) dy += distance;

        if (dx != 0 && dy != 0) {
            dx = dx / std::sqrt(2.0);
            dy = dy / std::sqrt(2.0);
        }
		
		if (x()+dx <= 0 || x()+dx >= sceneWidth-19 || y()+dy < 0 || y()+dy > sceneHeight-19) return;
		
        setPos(x() + dx, y() + dy);
    }

private:
    bool leftPressed;
    bool rightPressed;
    bool upPressed;
    bool downPressed;
	bool firing;
    QTimer* timer;
	short distance;
};
Player* player;
//Player.end



//PlayerVCore.begin
class PlayerVCore : public QObject, public QGraphicsPixmapItem {
public:
    PlayerVCore(QGraphicsItem* parent): QGraphicsPixmapItem(parent), playerPointer(parent) {
        setPixmap(QPixmap("image/circle_red.png"));
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, [&] {
			for (QGraphicsItem* item : scene()->collidingItems(this)) {
				if (item != this && item != player) {
					RL--;
					playerPointer->setPos(startingPoint[0],startingPoint[1]);
				}
			}
		});
		timer->start(collidingDetectionConstant);
    }

private: QGraphicsItem* playerPointer;
};
//PlayerVCore.end



// ##################################################################################################################################################





void create(std::vector<CoordSet>queue) {
	std::thread([queue]{
		for (CoordSet set : queue) {
			while(true) {
				if (time_s >= set.t) {
					im([set] {new enemy_default(set.c);});
					break;}
				sleepc(tu::l,fpsConstant);
			}
		}
	}).detach();
}

std::string entityList[] = {
	"enemy_default",
	"bullet_track"
	#warning Dont forget bullet_track
};

void jsonRead(std::string filename) {
	std::vector<CoordSet> queue;
	std::ifstream file(filename);
	if (!file) throw std::runtime_error("json file NotFound\n");
	nlohmann::json jsonData;
	file >> jsonData;
	
	// RL
	RL = jsonData["RL"];
	
	// BezierCurve
	if (jsonData.contains("BezierCurveList")) {
        nlohmann::json bezierCurves = jsonData["BezierCurveList"];
        for (nlohmann::json::iterator it = bezierCurves.begin(); it != bezierCurves.end(); ++it) {
            std::vector<QPointF> points;
            for (const auto& pointData : it.value()) {
                QPointF point(pointData[0], pointData[1]);
                points.push_back(point);
            }
            BezierCurveList.push_back(points);
        }
    }
	
	
	// entity
	for (size_t et = 0; et < std::size(entityList); ++et) {
		nlohmann::json current = jsonData[entityList[et]];
		for (size_t i = 0; i < current.size(); ++i) {
			if (current[i][3].is_null()) {
				queue.emplace_back(CoordSet(current[i][0],current[i][1],current[i][2]));
			} else if (current[i][4].is_null()) {
				queue.emplace_back(CoordSet(current[i][0],current[i][1],current[i][2],current[i][3]));
			} else queue.emplace_back(CoordSet(current[i][0],current[i][1],current[i][2],current[i][3],current[i][4]));
		}
		
/*		//debug
		for (CoordSet c: queue) {
			std::cout << "x" << c.c.x << " y" << c.c.y << " t" << c.c.t << " d" << c.c.d << "\n";
		}
*/		
		create(queue);
	}
}





dthread eventloop(tu::l,fpsConstant, []{
// frame
	++time_s;

// bullet_player_default-move
	for (bullet_player_default *obj : bpd_instances) {
		/*for (int i = 1; i <= 10; i++) */im([obj] {
			obj->setPos(obj->x(), obj->y()-10);
		});
	}


// 残機・スコア
	if (RL <= 0) {
		player->deleteLater();
		im([]{
			MWStatusBar->setStyleSheet("color: red; font-size: 20px;");
			MWStatusBar->showMessage("GameOver   スコア" + QString::number(score) + "   [ゲームを終了してください]");
		});
		sleepc(tu::s,30);
		exit(0);
	}
	im([]{
		MWStatusBar->showMessage(QString("スコア: %1   残機: %2   |||DEBUG scene内オブジェクト数: %3   フレーム数: %4")
			.arg(score).arg(RL).arg(scene->items().size()).arg(time_s));
	});
});

/*
// DEBUG CLI処理部 refresh関数
dthread CLI(true,[]{
	while(true) {
		std::cin.get();
		// std::cout << scene->items().size();
		im([]{
			for (QGraphicsItem* item : scene->items()) {
				if (item->type() == basic::Type) {
					scene->removeItem(item);
					dynamic_cast<basic*>(item)->deleteLater();
				}
			}
		});
	}
});
*/


int main(int argc, char* argv[]) {
	/* GPU */
	QSurfaceFormat format;
    // format.setOption(QSurfaceFormat::DebugContext);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4,6);  // OpenGL4.6 2024/10/15
    QSurfaceFormat::setDefaultFormat(format);


	/* Qt_system */
	QApplication app(argc, argv);
	// ミューテックスを利用した多重起動防止機構
    HANDLE hMutex = CreateMutex(nullptr, TRUE, "Global\\ShootingAppMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
		QSystemTrayIcon trayIcon;
		trayIcon.setIcon(QIcon("image/trayIcon.png"));
		// アイコンをシステムトレイに表示
		trayIcon.setVisible(false);
		trayIcon.show();
		// バルーン通知を表示
		trayIcon.showMessage(QString::fromLocal8Bit("多重起動は禁止されています"), 
							 QString::fromLocal8Bit("既に実行されていたゲームを終了します\nもう一度ゲームを開きなおしてください"), QSystemTrayIcon::Warning, 10000);
		sleepc(tu::s,10);
		std::system("taskkill /im Shooting.exe");
		return 1;
	}


	/* 優先度 */
    // スレッド単位 -> if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST)) {std::cerr << "Failed to set main thread priority."; return 1;}
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);


	/* pixmap */
	sp::A_aqua = new QPixmap("image/A_aqua.png");
	sp::triangle_red = new QPixmap("image/triangle_red.png");
	sp::hexagon_yellow = new QPixmap("image/hexagon_yellow.png");
	sp::triangle_blue = new QPixmap("image/triangle_blue.png");
	if (sp::A_aqua->isNull()) throw std::runtime_error("画像を読み込めませんでした"); // 標本調査


	/* scene */
	scene = new QGraphicsScene;
	scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);  // インデックス不使用
	scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
	scene->setBackgroundBrush(QColor(32, 32, 32));
	bullet::setScene(scene);
	enemy::setScene(scene);


	/* view */
	QGraphicsView view(scene);
	view.setViewport(new QOpenGLWidget);
	view.setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
	view.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	view.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
	view.setFixedSize(sceneWidth, sceneHeight);
	   // view.setFixedSize(1000,1000);
	view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


	/* window */
	QMainWindow mainWindow;
	mainWindow.setCentralWidget(&view);
	MWStatusBar = mainWindow.statusBar();
	MWStatusBar->setStyleSheet("font-size: 20px;");
	mainWindow.setFixedSize(sceneWidth, sceneHeight + mainWindow.statusBar()->height());
	   // mainWindow.setFixedSize(1000, 1000);
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    mainWindow.move(screenGeometry.width()/2-sceneWidthHalf, screenGeometry.height()/2-sceneHeightHalf);


	/* player */
	bullet_player_default_relative = (sp::triangle_blue->width() - sp::hexagon_yellow->width())/2;
	startingPoint[0] = (sceneWidth - sp::triangle_blue->width()) /2;
	startingPoint[1] = sceneHeight - sp::triangle_blue->height()-5;
	player = new Player();
	PlayerVCore* playerVCore = new PlayerVCore(player);
	player->setPos(startingPoint[0],startingPoint[1]);
	playerVCore->setPos((player->pixmap().width() - playerVCore->pixmap().width())/2,  (player->pixmap().height() - playerVCore->pixmap().height()+10)/2);
	scene->addItem(player);
	player->setFlag(QGraphicsItem::ItemIsFocusable);
	player->setFocus();


	/* moveCache初期化 */
	for (int i = 0; i < 359; ++i) {
		float radians = qDegreesToRadians(i-90);
		moveCache[i][0] = std::cos(radians);
		moveCache[i][1] = std::sin(radians);
    }


	/* json */
	std::string jsonfilename;
	if (argc==2) jsonfilename = std::string(argv[1]); else jsonfilename = "shooting.json";
	jsonRead(jsonfilename);


	/* ウィンドウ表示&アプリ実行 */
	mainWindow.show();
	int appExecReturn = app.exec();
	ReleaseMutex(hMutex);
    CloseHandle(hMutex);
	return appExecReturn;
}
#include "Shooting.moc"
