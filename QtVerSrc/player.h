#pragma once
#include <QtCore/QObject>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtGui/QKeyEvent>
#include "custom.entity.h"


//Player.begin
class Player : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Player();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
	void focusOutEvent(QFocusEvent *event) override;

private slots:
    void updatePosition();

private:
    bool leftPressed;
    bool rightPressed;
    bool upPressed;
    bool downPressed;
	bool firing;
    QTimer* timer;
	short distance;
};
//Player.end
extern Player* player;



//PlayerVCore.begin
class PlayerVCore : public QObject, public QGraphicsPixmapItem {
public:
    PlayerVCore(QGraphicsItem* parent);

private: QGraphicsItem* playerPointer;
};
//PlayerVCore.end


