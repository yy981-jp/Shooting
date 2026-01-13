#include <windows.h>
#include <QtGui/QSurfaceFormat>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>

#include <yy981/time.h>

#include "def.h"
#include "basic.entity.h"
#include "main.h"
#include "create.h"
#include "player.h"


int main(int argc, char* argv[]) {
	/* GPU */
	QSurfaceFormat format;
    // format.setOption(QSurfaceFormat::DebugContext);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4,6);  // OpenGL4.6 2024/10/15
    QSurfaceFormat::setDefaultFormat(format);


	/* Qt_system */
	QApplication app(argc, argv);
	// ミューテックスを利用した多重起動防止機構
    HANDLE hMutex = CreateMutex(nullptr, TRUE, "Global\\ShootingAppMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
		QSystemTrayIcon trayIcon;
		trayIcon.setIcon(QIcon("image/trayIcon.png"));
		// アイコンをシステムトレイに表示
		trayIcon.setVisible(false);
		trayIcon.show();
		// バルーン通知を表示
		trayIcon.showMessage(QString::fromLocal8Bit("多重起動は禁止されています"), 
							 QString::fromLocal8Bit("既に実行されていたゲームを終了します\nもう一度ゲームを開きなおしてください"), QSystemTrayIcon::Warning, 10000);
		sleepc(tu::s,10);
		std::system("taskkill /im Shooting.exe");
		return 1;
	}


	/* 優先度 */
    // スレッド単位 -> if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST)) {std::cerr << "Failed to set main thread priority."; return 1;}
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);


	/* pixmap */
	sp::A_aqua = new QPixmap("image/A_aqua.png");
	sp::triangle_red = new QPixmap("image/triangle_red.png");
	sp::hexagon_yellow = new QPixmap("image/hexagon_yellow.png");
	sp::triangle_blue = new QPixmap("image/triangle_blue.png");
	if (sp::A_aqua->isNull()) throw std::runtime_error("画像を読み込めませんでした"); // 標本調査


	/* scene */
	scene = new QGraphicsScene;
	scene->setItemIndexMethod(QGraphicsScene::BspTreeIndex);  // インデックス不使用
	scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
	scene->setBackgroundBrush(QColor(32, 32, 32));
	bullet::setScene(scene);
	enemy::setScene(scene);


	/* view */
	QGraphicsView view(scene);
	view.setViewport(new QOpenGLWidget);
	view.setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
	view.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	view.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
	view.setFixedSize(sceneWidth, sceneHeight);
	   // view.setFixedSize(1000,1000);
	view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


	/* window */
	QMainWindow mainWindow;
	mainWindow.setCentralWidget(&view);
	MWStatusBar = mainWindow.statusBar();
	MWStatusBar->setStyleSheet("font-size: 20px;");
	mainWindow.setFixedSize(sceneWidth, sceneHeight + mainWindow.statusBar()->height());
	   // mainWindow.setFixedSize(1000, 1000);
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    mainWindow.move(screenGeometry.width()/2-sceneWidthHalf, screenGeometry.height()/2-sceneHeightHalf);


	/* player */
	bullet_player_default_relative = (sp::triangle_blue->width() - sp::hexagon_yellow->width())/2;
	startingPoint[0] = (sceneWidth - sp::triangle_blue->width()) /2;
	startingPoint[1] = sceneHeight - sp::triangle_blue->height()-5;
	player = new Player();
	PlayerVCore* playerVCore = new PlayerVCore(player);
	player->setPos(startingPoint[0],startingPoint[1]);
	playerVCore->setPos((player->pixmap().width() - playerVCore->pixmap().width())/2,  (player->pixmap().height() - playerVCore->pixmap().height()+10)/2);
	scene->addItem(player);
	player->setFlag(QGraphicsItem::ItemIsFocusable);
	player->setFocus();


	/* moveCache初期化 */
	for (int i = 0; i < 359; ++i) {
		float radians = qDegreesToRadians(i-90);
		moveCache[i][0] = std::cos(radians);
		moveCache[i][1] = std::sin(radians);
    }


	/* json */
	std::string jsonfilename;
	if (argc==2) jsonfilename = std::string(argv[1]); else jsonfilename = "shooting.json";
	jsonRead(jsonfilename);


	/* ウィンドウ表示&アプリ実行 */
	mainWindow.show();
	int appExecReturn = app.exec();
	ReleaseMutex(hMutex);
    CloseHandle(hMutex);
	return appExecReturn;
}
