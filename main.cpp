#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <unordered_map>

#include "def.h"
#include "basic.entity.h"
#include "main.h"
#include "create.h"
#include "player.h"

int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Shooting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sceneWidth, sceneHeight, SDL_WINDOW_SHOWN);
	if (!window) { std::cerr << "CreateWindow failed: " << SDL_GetError() << std::endl; return 1; }
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) { std::cerr << "CreateRenderer failed: " << SDL_GetError() << std::endl; return 1; }
	qt::setRenderer(renderer);

	// load pixmaps
	sp::A_aqua = new QPixmap("image/A_aqua.png");
	sp::triangle_red = new QPixmap("image/triangle_red.png");
	sp::hexagon_yellow = new QPixmap("image/hexagon_yellow.png");
	sp::triangle_blue = new QPixmap("image/triangle_blue.png");
	if (sp::A_aqua->isNull()) throw std::runtime_error("画像を読み込めませんでした");

	// scene
	scene = new QGraphicsScene;
	scene->setSceneRect(0,0,sceneWidth,sceneHeight);
	scene->setBackgroundBrush(QColor(32,32,32));
	bullet::setScene(scene);
	enemy::setScene(scene);

	// player
	bullet_player_default_relative = (sp::triangle_blue->width() - sp::hexagon_yellow->width())/2;
	startingPoint[0] = (sceneWidth - sp::triangle_blue->width()) /2;
	startingPoint[1] = sceneHeight - sp::triangle_blue->height()-5;
	player = new Player();
	PlayerVCore* playerVCore = new PlayerVCore(player);
	player->setPos(startingPoint[0],startingPoint[1]);
	playerVCore->setPos((player->pixmap().width() - playerVCore->pixmap().width())/2,  (player->pixmap().height() - playerVCore->pixmap().height()+10)/2);
	scene->addItem(player);

	// texture cache
	std::unordered_map<SDL_Surface*, SDL_Texture*> texcache;

	bool running = true;
	SDL_Event e;
	uint32_t last = SDL_GetTicks();
	while (running) {
		uint32_t now = SDL_GetTicks();
		int dt = static_cast<int>(now - last);
		last = now;
		tick_timers(dt);

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
				bool down = (e.type == SDL_KEYDOWN);
				int key = e.key.keysym.sym;
				int qk = 0;
				switch (key) {
					case SDLK_LEFT: qk = Qt::Key_Left; break;
					case SDLK_RIGHT: qk = Qt::Key_Right; break;
					case SDLK_UP: qk = Qt::Key_Up; break;
					case SDLK_DOWN: qk = Qt::Key_Down; break;
					case SDLK_LSHIFT: case SDLK_RSHIFT: qk = Qt::Key_Shift; break;
					case SDLK_z: qk = Qt::Key_Z; break;
					default: qk = key; break;
				}
				QKeyEvent qev(qk);
				if (player) {
					if (down) player->keyPressEvent(&qev); else player->keyReleaseEvent(&qev);
				}
			}
		}

		// render
		SDL_SetRenderDrawColor(renderer, 32,32,32,255);
		SDL_RenderClear(renderer);
		for (auto item : scene->items()) {
			QGraphicsPixmapItem* pi = dynamic_cast<QGraphicsPixmapItem*>(item);
			if (!pi) continue;
			QPixmap pm = pi->pixmap();
			if (pm.surf==nullptr) continue;
			SDL_Texture* tex = nullptr;
			auto it = texcache.find(pm.surf);
			if (it!=texcache.end()) tex = it->second; else {
				tex = SDL_CreateTextureFromSurface(renderer, pm.surf);
				if (tex) texcache[pm.surf] = tex;
			}
			if (tex) {
				SDL_Rect dst{ static_cast<int>(item->x()), static_cast<int>(item->y()), pm.width(), pm.height() };
				SDL_RenderCopy(renderer, tex, nullptr, &dst);
			}
		}
		SDL_RenderPresent(renderer);

		SDL_Delay(1);
	}

	// cleanup
	for (auto &p : texcache) if (p.second) SDL_DestroyTexture(p.second);
	delete sp::A_aqua; delete sp::triangle_red; delete sp::hexagon_yellow; delete sp::triangle_blue;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}
