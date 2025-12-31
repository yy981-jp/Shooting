#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>

#include <vector>
#include <functional>
#include <unordered_set>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <stdexcept>
#include <memory>

// Minimal Qt compatibility layer backed by SDL2 for this project.

#define signals public
#define slots
#define emit

constexpr int UserType = 65536;

struct QPointF { 
    double x_, y_;
    QPointF(double a=0,double b=0):x_(a),y_(b){}
    void setX(double v){ x_ = v; }
    void setY(double v){ y_ = v; }
    double X() const { return x_; }
    double Y() const { return y_; }
    double x() const { return x_; }
    double y() const { return y_; }
};

inline QPointF operator+(const QPointF& a, const QPointF& b) { return QPointF(a.x() + b.x(), a.y() + b.y()); }
inline QPointF operator-(const QPointF& a, const QPointF& b) { return QPointF(a.x() - b.x(), a.y() - b.y()); }
inline QPointF operator*(double s, const QPointF& p) { return QPointF(p.x() * s, p.y() * s); }
inline QPointF operator*(const QPointF& p, double s) { return QPointF(p.x() * s, p.y() * s); }
inline QPointF operator*(int s, const QPointF& p) { return QPointF(p.x() * s, p.y() * s); }
inline QPointF operator*(const QPointF& p, int s) { return QPointF(p.x() * s, p.y() * s); }
inline QPointF operator/(const QPointF& p, double s) { return QPointF(p.x() / s, p.y() / s); }
inline QPointF& operator+=(QPointF& a, const QPointF& b) { a = a + b; return a; }

inline void disconnect() {}

struct QFocusEvent {};

struct QColor { int r,g,b; QColor(int rr=0,int gg=0,int bb=0):r(rr),g(gg),b(bb){} };

class QObject {
public:
    QObject(void* = nullptr) {}
    void deleteLater() { delete this; }
    template<typename A, typename B, typename C>
    static void connect(A*, B, C) { /* no-op compatibility shim */ }
    template<typename A, typename B, typename C, typename D>
    static void connect(A*, B, C, D) { /* no-op 4-arg overload */ }
};

namespace Qt {
    enum Key {
        Key_Left = 0x01000012,
        Key_Right = 0x01000013,
        Key_Up = 0x01000014,
        Key_Down = 0x01000015,
        Key_Shift = 0x01000020,
        Key_Z = 0x5A
    };
    enum ScrollBarPolicy { ScrollBarAlwaysOff };
}

class QKeyEvent {
public:
    QKeyEvent(int k): k_(k) {}
    int key() const { return k_; }
private:
    int k_;
};

// Forward declarations
class QGraphicsItem;
class QGraphicsScene;

// Simple pixmap wrapper (loads SDL_Surface via SDL_image)
class QPixmap {
public:
    QPixmap(): surf(nullptr), w(0), h(0) {}
    QPixmap(const char* path) { load(path); }
    ~QPixmap() { if (surf) SDL_FreeSurface(surf); }
    bool isNull() const { return surf==nullptr; }
    int width() const { return w; }
    int height() const { return h; }
    bool load(const char* path) {
        if (surf) { SDL_FreeSurface(surf); surf = nullptr; }
        surf = IMG_Load(path);
        if (!surf) return false;
        w = surf->w; h = surf->h; return true;
    }
    SDL_Surface* surf;
private:
    int w,h;
};

// Basic geometry and item classes
class QRectF {
public:
    QRectF() : x(0), y(0), w(0), h(0) {}
    QRectF(double ix, double iy, double iw, double ih): x(ix), y(iy), w(iw), h(ih) {}
    QRectF translated(const QPointF& p) const { return QRectF(x + p.x(), y + p.y(), w, h); }
    QRectF united(const QRectF& o) const { double nx = std::min(x, o.x); double ny = std::min(y, o.y); double nx2 = std::max(x+w, o.x+o.w); double ny2 = std::max(y+h, o.y+o.h); return QRectF(nx, ny, nx2-nx, ny2-ny); }
    double x,y,w,h;
};

class QGraphicsItem {
public:
    enum { ItemCoordinateCache = 0, ItemDeviceCoordinateCache = 1, ItemIsFocusable = 2 };
    QGraphicsItem(QGraphicsItem* parent=nullptr): x_(0), y_(0), parent_(parent) {}
    virtual ~QGraphicsItem() {}
    void setPos(double x,double y) { x_ = x; y_ = y; }
    void setPos(const QPointF& p) { setPos(p.x(), p.y()); }
    void setPos(int x,int y) { setPos(static_cast<double>(x), static_cast<double>(y)); }
    double x() const { return x_; }
    double y() const { return y_; }
    QPointF pos() const { return QPointF(x_, y_); }
    virtual void focusOutEvent(QFocusEvent* /*event*/) {}
    virtual void keyPressEvent(QKeyEvent*) {}
    virtual void keyReleaseEvent(QKeyEvent*) {}
    virtual int type() const { return 0; }
    static constexpr int UserType = ::UserType;
    void setParentItem(QGraphicsItem* p) { parent_ = p; }
    QGraphicsScene* scene() { return nullptr; }
    virtual QRectF boundingRect() const { return QRectF(x_, y_, 0, 0); }
    void setCacheMode(int) {}
    void setRotation(double) {}
    double rotation() const { return 0.0; }
protected:
    double x_, y_;
    QGraphicsItem* parent_;
};

class QGraphicsPixmapItem : public QGraphicsItem {
public:
    QGraphicsPixmapItem(QGraphicsItem* parent=nullptr): QGraphicsItem(parent) {}
    void setPixmap(const QPixmap& p) { pix = p; }
    QPixmap pixmap() const { return pix; }
    void setFlag(int) {}
    void setFocus() {}
    using QGraphicsItem::setPos;
    void setPos(const QPointF& p) { QGraphicsItem::setPos(p.x(), p.y()); }
private:
    QPixmap pix;
};

// Scene: simple container + collision
class QGraphicsScene {
public:
    void addItem(QGraphicsItem* i) { items_.push_back(i); }
    std::vector<QGraphicsItem*> items() { return items_; }
    std::vector<QGraphicsItem*> items(const QRectF& r) {
        std::vector<QGraphicsItem*> res;
        for (auto it : items_) {
            QRectF b = it->boundingRect().translated(it->pos());
            // simple intersection test
            if (!(b.x + b.w < r.x || r.x + r.w < b.x || b.y + b.h < r.y || r.y + r.h < b.y)) res.push_back(it);
        }
        return res;
    }
    void removeItem(QGraphicsItem* i) { items_.erase(std::remove(items_.begin(), items_.end(), i), items_.end()); }
    std::vector<QGraphicsItem*> collidingItems(QGraphicsItem* it) {
        std::vector<QGraphicsItem*> r;
        // bounding box collision using pixmap size if available
        QGraphicsPixmapItem* pi = dynamic_cast<QGraphicsPixmapItem*>(it);
        if (!pi) return r;
        int ix = static_cast<int>(it->x());
        int iy = static_cast<int>(it->y());
        int iw = pi->pixmap().width();
        int ih = pi->pixmap().height();
        for (auto o : items_) {
            if (o==it) continue;
            QGraphicsPixmapItem* po = dynamic_cast<QGraphicsPixmapItem*>(o);
            if (!po) continue;
            int ox = static_cast<int>(o->x());
            int oy = static_cast<int>(o->y());
            int ow = po->pixmap().width();
            int oh = po->pixmap().height();
            if (ix < ox+ow && ix+iw > ox && iy < oy+oh && iy+ih > oy) r.push_back(o);
        }
        return r;
    }
    void setSceneRect(int, int, int, int) {}
    void setBackgroundBrush(const QColor& c) { bg = c; }
private:
    std::vector<QGraphicsItem*> items_;
    QColor bg;
};

// Status bar stub
class QStatusBar {
public:
    void setStyleSheet(const char*) {}
    int height() const { return 22; }
    void showMessage(const std::string& msg) {}
};

// System tray stub
class QSystemTrayIcon {
public:
    void setIcon(const char*) {}
    void setVisible(bool) {}
    void show() {}
    enum MessageIcon { Warning };
    void showMessage(const std::string& title, const std::string& msg, MessageIcon, int) {}
};

// Simple QString shim
class QString {
public:
    QString() {}
    QString(const char* s): s_(s) {}
    QString(const std::string& s): s_(s) {}
    static QString fromLocal8Bit(const char* s) { return QString(s); }
    static QString number(int v) { return QString(std::to_string(v)); }
    QString arg(int v) const { return QString(s_ + std::to_string(v)); }
    std::string toStdString() const { return s_; }
    QString operator+(const QString& o) const { return QString(s_ + o.s_); }
    operator std::string() const { return s_; }
private:
    std::string s_;
};

inline QString operator+(const char* a, const QString& b) { return QString(std::string(a) + b.toStdString()); }
inline QString operator+(const std::string& a, const QString& b) { return QString(a + b.toStdString()); }

// Simple timer manager
class QTimer {
public:
    QTimer(QObject* = nullptr) {}
    void start(int ms) { interval = ms; registerTimer(); }
    void stop() { interval = 0; }
    void timeout() {}
    void addCallback(std::function<void()> cb) { callbacks.push_back(cb); }
    int interval{0};
    static void singleShot(int ms, QObject* /*obj*/, std::function<void()> cb) {
        QTimer* t = new QTimer();
        t->addCallback([cb,t]{ cb(); delete t; });
        t->start(ms);
    }
private:
    std::vector<std::function<void()>> callbacks;
    void registerTimer();
    friend void tick_timers(int);
};

// Global timer registry
struct TimerEntry { std::shared_ptr<QTimer> timer; std::chrono::steady_clock::time_point next; int interval; std::vector<std::function<void()>> callbacks; };

static std::vector<TimerEntry> g_timers;
static std::mutex g_timers_mtx;

inline void QTimer::registerTimer() {
    std::lock_guard<std::mutex> lk(g_timers_mtx);
    TimerEntry e;
    e.timer = std::make_shared<QTimer>(*this);
    e.interval = interval;
    e.next = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
    e.callbacks = callbacks;
    g_timers.push_back(std::move(e));
}

inline void tick_timers(int ms) {
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lk(g_timers_mtx);
    for (auto &e : g_timers) {
        if (e.interval<=0) continue;
        if (now >= e.next) {
            e.next = now + std::chrono::milliseconds(e.interval);
            for (auto &cb : e.callbacks) {
                try { cb(); } catch(...) {}
            }
        }
    }
}

// simplified connect helper for QTimer -> member function
template<typename Sender, typename Signal, typename Receiver, typename Slot>
void connect(Sender* sender, Signal /*sig*/, Receiver* receiver, Slot slot) {
    // sender is expected to be QTimer*
    QTimer* t = reinterpret_cast<QTimer*>(sender);
    t->addCallback([receiver, slot]() { (receiver->*slot)(); });
}

// Helper to set global SDL_Renderer for drawing (main should set this)
namespace qt {
    inline SDL_Renderer* g_renderer = nullptr;
    inline void setRenderer(SDL_Renderer* r) { g_renderer = r; }
}
