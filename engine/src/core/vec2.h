struct Vec2 {
    int x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}

    Vec2(const Vec2F& vf) {
        x = vf.x;
        y = vf.y;
    }
    Vec2& operator=(const Vec2F& vf) {
        x = vf.x;
        y = vf.y;
        return *this;
    }
};

struct Vec2F {
    float x, y;
    Vec2F() : x(0), y(0) {}
    Vec2F(float x, float y) : x(x), y(y) {}
    Vec2F(const Vec2& v) {
        x = v.x;
        y = v.y;
    }
    Vec2F& operator=(const Vec2& v) {
        x = v.x;
        y = v.y;
        return *this;
    }
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
