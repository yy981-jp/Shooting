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


#include "Shooting.moc"
