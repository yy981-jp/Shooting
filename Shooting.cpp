	/* ---STD--- */
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <thread>
#include <functional>
#include <fstream>
#include <unordered_set>
// #include <mutex>

	/* ---OTHER--- */
#include <nlohmann/json.hpp>
#include <boost/signals2/signal.hpp>
#include <yy981/debug.h>
#include <yy981/thread.h>

	/* ---WINDOWS--- */
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

	/* ---QT--- */
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QSequentialAnimationGroup>
#include <QtCore/QPointF>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLabel>
#include <QtGui/QKeyEvent>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtGui/QPainterPath>
#include <QtGui/QSurfaceFormat>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "def.h"
#include "BezierMover.h"
#include "basic.entity.h"
#include "custom.entity.h"
#include "player.h"
#include "create.h"
#include "main.h"



#ifdef DEBUG
	void DBG(const std::string str) {std::cout << "DEBUG::: " << str << "\n";}
#else
	void DBG(const std::string str) {}
#endif

// --------------------------------------------------------------------------------------------------------------------------------------------------
			/* custom object */





dthread eventloop(tu::l,fpsConstant, []{
// frame
	++time_s;

// bullet_player_default-move
	for (bullet_player_default *obj : bpd_instances) {
		/*for (int i = 1; i <= 10; i++) */im([obj] {
			obj->setPos(obj->x(), obj->y()-10);
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


#include "Shooting.moc"
