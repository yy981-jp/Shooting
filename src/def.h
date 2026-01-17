#pragma once


struct Vec2 {int x, y;};
struct Vec2F {
    float x, y;
    Vec2F() : x(0), y(0) {}
    Vec2F(float x, float y) : x(x), y(y) {}
    Vec2F operator+(const Vec2F& other) const {
        return Vec2F(x + other.x, y + other.y);
    }
    Vec2F operator*(float scalar) const {
        return Vec2F(x * scalar, y * scalar);
    }
    Vec2F& operator+=(const Vec2F& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};
inline Vec2F operator*(float scalar, const Vec2F& p) {
    return p * scalar;
}
inline Vec2F operator*(const Vec2& p, float scalar) {
    return Vec2F(p.x * scalar, p.y * scalar);
}

