#include "vec2.h"
#include <math.h>

// Constructors and destructor   ...............................................
Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin) {}

Vec2::Vec2(const sf::Vector2f& rhs)
    : x(rhs.x), y(rhs.y) {}

Vec2::Vec2(const sf::Vector2i& rhs)
    : x(rhs.x), y(rhs.y) {}

Vec2::~Vec2() {}

// Bool return type   ..........................................................
bool Vec2::operator == (const Vec2& rhs) const
{
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator != (const Vec2& rhs) const
{
    return x != rhs.x && y != rhs.y;
}

bool Vec2::operator <= (const Vec2& rhs) const
{
    return x <= rhs.x && y <= rhs.y;
}

bool Vec2::operator >= (const Vec2& rhs) const
{
    return x >= rhs.x && y >= rhs.y;
}

bool Vec2::operator < (const Vec2& rhs) const
{
    return x < rhs.x && y < rhs.y;
}

bool Vec2::operator > (const Vec2& rhs) const
{
    return x > rhs.x && y > rhs.y;
}

// Vec2 return type   ..........................................................
Vec2 Vec2::operator + (const Vec2& rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const Vec2& rhs) const
{
    return Vec2(x * rhs.x, y * rhs.y);
}

Vec2 Vec2::operator / (const Vec2& rhs) const
{
    return Vec2(x / rhs.x, y / rhs.y);
}

Vec2 Vec2::operator + (const sf::Vector2f& rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const sf::Vector2f& rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const sf::Vector2f& rhs) const
{
    return Vec2(x * rhs.x, y * rhs.y);
}

Vec2 Vec2::operator / (const sf::Vector2f& rhs) const
{
    return Vec2(x / rhs.x, y / rhs.y);
}

Vec2 Vec2::operator + (const int val) const
{
    return Vec2(x + val, y + val);
}

Vec2 Vec2::operator - (const int val) const
{
    return Vec2(x - val, y - val);
}

Vec2 Vec2::operator * (const int val) const
{
    return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const int val) const
{
    return Vec2(x / val, y / val);
}

Vec2 Vec2::operator + (const float val) const
{
    return Vec2(x + val, y + val);
}

Vec2 Vec2::operator - (const float val) const
{
    return Vec2(x - val, y - val);
}

Vec2 Vec2::operator * (const float val) const
{
    return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const
{
    return Vec2(x / val, y / val);
}

// void return type   ..........................................................
void Vec2::operator += (const Vec2& rhs)
{
    x += rhs.x; y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
    x -= rhs.x; y -= rhs.y;
}

void Vec2::operator *= (const Vec2& rhs)
{
    x *= rhs.x; y *= rhs.y;
}

void Vec2::operator /= (const Vec2& rhs)
{
    x /= rhs.x; y /= rhs.y;
}

void Vec2::operator += (const float val)
{
    x += val; y += val;
}

void Vec2::operator -= (const float val)
{
    x -= val; y -= val;
}

void Vec2::operator *= (const float val)
{
    x *= val; y *= val;
}

void Vec2::operator /= (const float val)
{
    x /= val; y /= val;
}

void Vec2::operator = (const float val)
{
    x = val; y = val;
}

void Vec2::operator = (const sf::Vector2f& rhs)
{
    x = rhs.x; y = rhs.y;
}

void Vec2::operator = (const sf::Vector2i& rhs)
{
    x = rhs.x; y = rhs.y;
}

// dist function   .............................................................
float Vec2::dist(const Vec2& rhs) const
{
    return sqrtf(pow(rhs.x - x, 2) + pow(rhs.y - y, 2));
}