#include <QtCore/QPointF>
#include "def.h"
#include "BezierMover.h"
#include "custom.entity.h"


	bullet_arrow::bullet_arrow(const Coord& coord): bullet(coord) {setPixmap(*sp::A_aqua);}



	enemy_default::enemy_default(const Coord& i_coord): coord(i_coord), enemy(i_coord) {process();}
	void enemy_default::process() {
		setPixmap(*sp::triangle_red);
		std::vector<QPointF> controlPoints = BezierCurveList[coord.d];
		for (QPointF& current : controlPoints) {
			current.setX(current.x()+x());
			current.setY(current.y()+y());
		}
		for (QPointF& current : controlPoints) controlPoints.insert(controlPoints.begin(),QPointF(x(),y()));
			// std::cout << "[ct]: " << coord.t << " [02]: " << subnum(coord.t,0,2,true) << " [20]: " << subnum(coord.t,2,0,true) << "\n";

		new BezierMover(this, controlPoints, BML {
			if (L%30==10) for (int i = -1; i <= 1; i++) {
				new bullet_arrow(Coord(I->x()+i,I->y(),10,i*7));
			};
			// if (L%15==0) new bullet_arrow(Coord(I->x(),I->y()));
		},coord.t*1000);
	}
