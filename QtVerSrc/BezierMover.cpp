#include "BezierMover.h"
#include "basic.entity.h"


    BezierMover::BezierMover(QGraphicsPixmapItem* item, const std::vector<QPointF>& controlPoints, std::function<void(int,QGraphicsPixmapItem*)> i_f,
				int duration): QObject(nullptr), item(item), controlPoints(controlPoints), duration(duration), t(0.0), f(i_f) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &BezierMover::updatePosition);
        timer->start(fpsConstant); // 約60fpsで更新
		QObject::connect(dynamic_cast<enemy*>(item), &enemy::collisionDetected, [this]{this->deleteLater();});

    }

    void BezierMover::updatePosition() {
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

    QPointF BezierMover::calculateBezierPoint(double t) {
        int n = controlPoints.size() - 1;
        QPointF point(0, 0);
        for (int i = 0; i <= n; ++i) {
            double binomialCoeff = factorial(n) / (factorial(i) * factorial(n - i));
            double term = binomialCoeff * std::pow(1 - t, n - i) * std::pow(t, i);
            point += term * controlPoints[i];
        }
        return point;
    }

    int BezierMover::factorial(int n) {
        return (n <= 1) ? 1 : n * factorial(n - 1);
    }
