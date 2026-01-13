#pragma once
#include <vector>
#include <functional>
#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QTimer>
#include <QtWidgets/QGraphicsPixmapItem>

#include "def.h"

class BezierMover : public QObject {
Q_OBJECT
public:
    BezierMover(QGraphicsPixmapItem* item, const std::vector<QPointF>& controlPoints, std::function<void(int,QGraphicsPixmapItem*)> i_f = nullptr,
				int duration = 5000);
private slots:
    void updatePosition();

private:
    QPointF calculateBezierPoint(double t);

    int factorial(int n);

    QGraphicsPixmapItem* item;
    std::vector<QPointF> controlPoints;
    QTimer* timer;
    double t;
    int duration;
	short l = 0;
	std::function<void(int,QGraphicsPixmapItem*)> f;
};