#include "entity.basic.h"


	bullet::bullet(const Coord& coord, bool enableMove) {
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
	bullet::~bullet() {disconnect();}
	void bullet::setScene(QGraphicsScene* scene) {m_scene=scene;}

	void bullet::move() {
		int rotate = static_cast<int>(rotation());

		// x方向とy方向に移動
		setPos(x() + moveCache[rotate][0], y() + moveCache[rotate][1]);

		// 画面外に出たら削除
		if (x() < 0 || x() > sceneWidth || y() < 0 || y() > sceneHeight) {
			deleteLater();
			m_scene->removeItem(this);
		}
	}

QGraphicsScene* bullet::m_scene = nullptr;

	enemy::enemy(const Coord& coord) {
		setCacheMode(QGraphicsItem::ItemCoordinateCache);
		setPos(coord.x,coord.y);
		setRotation(180);
		m_scene->addItem(this);
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, [&] {
			QRectF currentRect = boundingRect().translated(pos());
			QRectF nextRect = boundingRect().translated(pos() + QPointF(0, -10));
			QRectF path = currentRect.united(nextRect);
			for (QGraphicsItem* item : m_scene->items(path)) {
				if (item->type() == bullet_player_default::Type) { // bullet_player_defaultか判定
					++score;
					emit collisionDetected();
					m_scene->removeItem(this);
					deleteLater();
				}
			}
		});
		timer->start(collidingDetectionConstant);
	}
	enemy::~enemy() {disconnect();}

	void enemy::setScene(QGraphicsScene* scene) {m_scene=scene;}

	void enemy::move(int distance, int interval) {
		int rotate = static_cast<int>(rotation());
		
		// x方向とy方向に移動
		for (int i = 1; i <= 10; i++) {QThread::msleep(interval);setPos(x() + moveCache[rotate][0], y() + moveCache[rotate][1]);}

		// 画面外に出たら削除
		if (x() < 0 || x() > sceneWidth || y() < 0 || y() > sceneHeight) {
			deleteLater();
			m_scene->removeItem(this);
		}
	}
	
	void enemy::process() {}

QGraphicsScene* enemy::m_scene = nullptr;


	bullet_player_default::bullet_player_default(const Coord& coord): bullet(coord,false) {
		setPixmap(*sp::hexagon_yellow);
		bpd_instances.emplace(this);
		QTimer::singleShot(1500, this, [this]{
			bpd_instances.erase(this);
			this->deleteLater();
		});
	}

	int bullet_player_default::type() const { return Type; }