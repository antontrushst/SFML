#pragma once

#include <SFML/Graphics.hpp>

class Vec2
{
public:
    float x = 0.f,
          y = 0.f;

    Vec2();
    Vec2(float xin, float yin);
    Vec2(const sf::Vector2f& rhs);
    Vec2(const sf::Vector2i& rhs);
    ~Vec2();

    bool operator == (const Vec2& rhs) const,
         operator != (const Vec2& rhs) const,
         operator <= (const Vec2& rhs) const,
         operator >= (const Vec2& rhs) const,
         operator <  (const Vec2& rhs) const,
         operator >  (const Vec2& rhs) const;

    Vec2 operator +  (const Vec2& rhs) const,
         operator -  (const Vec2& rhs) const,
         operator *  (const Vec2& rhs) const,
         operator /  (const Vec2& rhs) const,
         operator +  (const sf::Vector2f& rhs) const,
         operator -  (const sf::Vector2f& rhs) const,
         operator *  (const sf::Vector2f& rhs) const,
         operator /  (const sf::Vector2f& rhs) const,
         operator +  (const int   rhs) const,
         operator -  (const int   rhs) const,
         operator *  (const int   rhs) const,
         operator /  (const int   rhs) const,
         operator +  (const float val) const,
         operator -  (const float val) const,
         operator *  (const float val) const,
         operator /  (const float val) const;
    
    void operator += (const Vec2& rhs),
         operator -= (const Vec2& rhs),
         operator *= (const Vec2& rhs),
         operator /= (const Vec2& rhs),
         operator += (const float val),
         operator -= (const float val),
         operator *= (const float val),
         operator /= (const float val),
         operator =  (const float val),
         operator =  (const sf::Vector2f& rhs),
         operator =  (const sf::Vector2i& rhs);

    float dist(const Vec2& rhs) const;
};