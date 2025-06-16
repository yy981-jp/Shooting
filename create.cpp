#include <thread>
#include <fstream>

#include <nlohmann/json.hpp>

#include <yy981/time.h>

#include "create.h"


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
