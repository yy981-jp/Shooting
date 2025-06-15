#pragma once
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>

#include "def.h"


//basic
class basic: public QObject, public QGraphicsPixmapItem {
public:
	enum { Type = UserType + 1 };
};

class bullet: public basic {
public:
	bullet(const Coord& coord, bool enableMove = true);
	virtual ~bullet();
	static void setScene(QGraphicsScene* scene);
	virtual void move();
	static QGraphicsScene* m_scene;
};

class bullet_player_default: public bullet {
public:
	bullet_player_default(const Coord& coord);	
	constexpr static int Type = UserType + 2;
	int type() const override;
};


class enemy: public basic {
Q_OBJECT
public:
	enemy(const Coord& coord);
	virtual ~enemy();
	static void setScene(QGraphicsScene* scene);

signals:
    void collisionDetected();

protected:
	virtual void process();
	virtual void move(int distance = 5, int interval = 100);
	static QGraphicsScene* m_scene;
};
