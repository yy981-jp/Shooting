#include <thread>
#include <QtCore/QTimer>
#include "player.h"
#include "def.h"


//Player.begin
    Player::Player(): leftPressed(false), rightPressed(false), upPressed(false), downPressed(false), distance(5), firing(false) {
        setPixmap(QPixmap("image/triangle_blue.png"));
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &Player::updatePosition);
        timer->start(fpsConstant); // 約60fpsで更新
		QTimer *fireTimer = new QTimer(this);
		connect(fireTimer, &QTimer::timeout, this, [this] {
			if (firing) {
				new bullet_player_default(Coord(x()+bullet_player_default_relative+10, y()-10,1));
			}
		});
		fireTimer->start(100);
    }

    void Player::keyPressEvent(QKeyEvent* event) {
        switch (event->key()) {
            case Qt::Key_Left: leftPressed = true; break;
            case Qt::Key_Right: rightPressed = true; break;
            case Qt::Key_Up: upPressed = true; break;
            case Qt::Key_Down: downPressed = true; break;
            case Qt::Key_Shift: distance = 2; break;
            case Qt::Key_Z: firing = true; break;
        }
    }

    void Player::keyReleaseEvent(QKeyEvent* event) {
        switch (event->key()) {
            case Qt::Key_Left: leftPressed = false; break;
            case Qt::Key_Right: rightPressed = false; break;
            case Qt::Key_Up: upPressed = false; break;
            case Qt::Key_Down: downPressed = false; break;
            case Qt::Key_Shift: distance = 5; break;
            case Qt::Key_Z: firing = false; break;
        }
    }
	
	void Player::focusOutEvent(QFocusEvent *event) {
        QGraphicsPixmapItem::focusOutEvent(event);
		setFocus();
    }

    void Player::updatePosition() {
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
//Player.end
Player* player;



//PlayerVCore.begin
    PlayerVCore::PlayerVCore(QGraphicsItem* parent): QGraphicsPixmapItem(parent), playerPointer(parent) {
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
//PlayerVCore.end


