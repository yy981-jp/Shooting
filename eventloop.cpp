#include "eventloop.h"
#define YHO


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
