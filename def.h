namespace sp {
	QPixmap* A_aqua;
	QPixmap* triangle_red;
	QPixmap* hexagon_yellow;
	QPixmap* triangle_blue;
}

//座標.begin
struct Coord {
	Coord(int i_x, int i_y, int i_t=10, int i_d=0): x(i_x+sceneWidthHalf), y(i_y+sceneHeightHalf), t(i_t), d(i_d) {}
	Coord(double i_x, double i_y, int i_t=10, int i_d=0): x(static_cast<int>(i_x)), y(static_cast<int>(i_y)), t(i_t), d(i_d) {}
	int x, y, d, t;
};
//座標.end

struct CoordSet {
	CoordSet(int time, int i_x, int i_y, int i_t=10, int i_d=0): t(time*10), c(i_x,i_y,i_t,i_d) {}
	int t;
	Coord c;
};
