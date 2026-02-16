#include "../core/def.h"
#include "../core/time.h"
#include "../core/cache.h"
#include "../graphics/gfx.h"
#include "../VM/VM.h"
#include "../player/player.h"
#include "../bullet/playerBullet.h"
#include "../bullet/simpleBullet.h"
#include "../enemy/enemyBezier.h"


vec2i makeDir(bool up, bool down, bool left, bool right) {
	return {
		(right ? 1 : 0) - (left ? 1 : 0),
		(down  ? 1 : 0) - (up   ? 1 : 0)
	};
}

class StageScene {
    void update() {
        if (!elapsedTime) elapsedTime.init();
        int deltatime = elapsedTime.get();
        // VM step
        if (vm->running) {
            auto vm_r = vm->step();
            switch (vm_r) {
                using enum VM::ReturnCode;
                case success: finished: break;
                case error: throw std::runtime_error("VMで何らかの異常が発生しました"); break;
                case finished: break; // 現状何もする必要はない
                case spawnRequest: commandExec(vm->gamecommand); break;
            }
        }

        // entity update
        vec2i d = makeDir(keyStat.up, keyStat.down, keyStat.left, keyStat.right);
        commandExec( player->update(deltatime, d.x, d.y, keyStat.shift, keyStat.z) );
        if (!player->isAllive()) running = false;
        playerBullet_Manager->update(deltatime);
        commandExec( enemyBezier_Manager->update(deltatime) );
        simpleBullet_Manager->update(deltatime);

        physWorld.step(); // 当たり判定
    }
    void draw() const {
        renderer->drawSprite(entityTable.get("background"), vec2i(-WINDOW.x,-WINDOW.y));
        player->draw(renderer);
        playerBullet_Manager->draw(renderer);
        enemyBezier_Manager->draw(renderer);
        simpleBullet_Manager->draw(renderer);

        // DEBUG
        physWorld.draw(renderer);
    }

    struct KeyStat {
        bool
            up      = false,
            down    = false,
            left    = false,
            right   = false,
            z       = false,
            shift   = false;
    } keyStat;

    struct commandExec_core {
        StageScene& stg;

        void operator()(const cmd::enemyBezier& c) const { stg.enemyBezier_Manager->generate(vec2f(c.x,c.y),c.pattern,c.duration); }
        void operator()(const cmd::simpleBullet& c) const { stg.simpleBullet_Manager->generate(c.pos,c.rotate,c.speed); }
        void operator()(const cmd::playerBullet& c) const { stg.playerBullet_Manager->generate(c.pos); }
    };

    VM* vm;
    Renderer* renderer;
    ElapsedTime elapsedTime;
    Cache* cache;
    FpsCounter fpsc;

    Player* player;
    PlayerBullet_Manager* playerBullet_Manager;
    EnemyBezier_Manager* enemyBezier_Manager;
    SimpleBullet_Manager* simpleBullet_Manager;

    const std::string stgdatpath = Assets + "main.stg.dat";
    bool running = true;

    void commandExec(const GameCommand& c) {
        if (!c.enable) return; // 有効ではないコマンドは弾く
        std::visit(commandExec_core{*this}, c.c);
    }
    void commandExec(const GameCommands& cs) {
        for (auto& c: cs) commandExec(c);
    }

public:
    StageScene(const int windowWidth, const int windowHeight) {
        cache = new Cache;
        player = new Player(renderer, 5.0f*60.0f);
        playerBullet_Manager = new PlayerBullet_Manager(renderer);
        enemyBezier_Manager = new EnemyBezier_Manager(renderer);
        simpleBullet_Manager = new SimpleBullet_Manager(renderer, *cache);
        vm = new VM(stgdatpath);

    }
    ~StageScene();

};
