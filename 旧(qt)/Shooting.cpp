// #define DEBUG
#include "shooting_system.h"


class BezierMover;
inline void BezierMoverStopperConnect(QGraphicsPixmapItem* item, BezierMover* this_p);
class BezierMover : public QObject {
    Q_OBJECT
public:
    BezierMover(QGraphicsPixmapItem* item, const std::vector<QPointF>& controlPoints, std::function<void(int,QGraphicsPixmapItem*)> i_f = nullptr,
				int duration = 5000, QObject* parent = nullptr): QObject(parent), item(item), controlPoints(controlPoints), duration(duration), t(0.0), f(i_f) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &BezierMover::updatePosition);
        timer->start(fpsConstant); // 約60fpsで更新
		BezierMoverStopperConnect(item,this);
    }

private slots:
    void updatePosition() {
		// std::cout << "[" << t << "]" << std::endl;
        t += 16.0 / duration;
        if (t > 1.0) {
            t = 1.0;
            timer->stop();
        }

        QPointF pos = calculateBezierPoint(t);
		l++;
		if (f != nullptr) f(l, item);
        item->setPos(pos);
    }

private:
    QPointF calculateBezierPoint(double t) {
        int n = controlPoints.size() - 1;
        QPointF point(0, 0);
        for (int i = 0; i <= n; ++i) {
            double binomialCoeff = factorial(n) / (factorial(i) * factorial(n - i));
            double term = binomialCoeff * std::pow(1 - t, n - i) * std::pow(t, i);
            point += term * controlPoints[i];
        }
        return point;
    }

    int factorial(int n) {
        return (n <= 1) ? 1 : n * factorial(n - 1);
    }

    QGraphicsPixmapItem* item;
    std::vector<QPointF> controlPoints;
    QTimer* timer;
    double t;
    int duration;
	short l = 0;
	std::function<void(int,QGraphicsPixmapItem*)> f;
};

//basic
class basic: public QObject, public QGraphicsPixmapItem {
public:
	enum { Type = UserType + 1 };
};

// ##################################################################################################################################################
			/* basic object */
class bullet: public basic {
public:
	bullet(const Coord& coord, bool enableMove = true) {
		setCacheMode(QGraphicsItem::ItemCoordinateCache);
		setPos(coord.x, coord.y);
		setRotation(180);
		m_scene->addItem(this);
		if (enableMove) {
			QTimer* timer = new QTimer(this);
			connect(timer, &QTimer::timeout, this, [this] {move();});
			timer->start(coord.t);
		}
	}
	virtual ~bullet() {disconnect();}
	static void setScene(QGraphicsScene* scene) {m_scene=scene;}

	virtual void move() {
		int rotate = static_cast<int>(rotation());

		// x方向とy方向に移動
		setPos(x() + moveCache[rotate][0], y() + moveCache[rotate][1]);

		// 画面外に出たら削除
		if (x() < 0 || x() > sceneWidth || y() < 0 || y() > sceneHeight) {
			deleteLater();
			m_scene->removeItem(this);
		}
	}

	static QGraphicsScene* m_scene;
};
QGraphicsScene* bullet::m_scene = nullptr;



inline bool isBulletPlayerDefault(QGraphicsItem* item);
class enemy: public basic {
Q_OBJECT
public:
	enemy(const Coord& coord) {
		setCacheMode(QGraphicsItem::ItemCoordinateCache);
		setPos(coord.x,coord.y);
		setRotation(180);
		m_scene->addItem(this);
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, [&] {
			for (QGraphicsItem* item : m_scene->collidingItems(this)) {
				if (isBulletPlayerDefault(item)) {
					++score;
					emit collisionDetected();
					deleteLater();
					m_scene->removeItem(this);
				}
			}
		});
		timer->start(collidingDetectionConstant);
	}
	virtual ~enemy() {disconnect();}

	static void setScene(QGraphicsScene* scene) {m_scene=scene;}

signals:
    void collisionDetected();

protected:
	virtual void process() {};
	virtual void move(int distance = 5, int interval = 100) {
		int rotate = static_cast<int>(rotation());
		
		// x方向とy方向に移動
		for (int i = 1; i <= 10; i++) {QThread::msleep(interval);setPos(x() + moveCache[rotate][0], y() + moveCache[rotate][1]);}

		// 画面外に出たら削除
		if (x() < 0 || x() > sceneWidth || y() < 0 || y() > sceneHeight) {
			deleteLater();
			m_scene->removeItem(this);
		}
	}

	static QGraphicsScene* m_scene;
};
QGraphicsScene* enemy::m_scene = nullptr;
inline void BezierMoverStopperConnect(QGraphicsPixmapItem* item, BezierMover* this_p) {
	enemy* enemyItem = dynamic_cast<enemy*>(item);
	QObject::connect(enemyItem, &enemy::collisionDetected, [this_p]{this_p->deleteLater();});
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



class bullet_player_default: public bullet {
public:
	bullet_player_default(const Coord& coord): bullet(coord,false) {
		setPixmap(*sp::hexagon_yellow);
		bpd_instances.emplace(this);
		QTimer::singleShot(1500, this, [this]{
			bpd_instances.erase(this);
			this->deleteLater();
		});
	}
	
	enum { Type = UserType + 1 };
	int type() const override {return Type;}
};
inline bool isBulletPlayerDefault(QGraphicsItem* item) {return qgraphicsitem_cast<bullet_player_default*>(item) != nullptr;}



// --------------------------------------------------------------------------------------------------------------------------------------------------
			/* Player */


//Player.begin
class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Player(): leftPressed(false), rightPressed(false), upPressed(false), downPressed(false), distance(5), firing(false) {
        setPixmap(QPixmap("Shooting/triangle_blue.png"));
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
        setPixmap(QPixmap("Shooting/circle_red.png"));
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
	time_s++;

// bullet_player_default-move
	for (bullet_player_default *obj : bpd_instances) {
		for (int i = 1; i <= 10; i++) im([obj] {
			obj->setPos(obj->x(), obj->y()-1);
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


#include "shooting_fmain.h"
#include "shooting.moc"
// #include "shooting_system.moc"