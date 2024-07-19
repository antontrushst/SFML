#pragma once

#include "vec2.h"
#include <SFML/Graphics.hpp>

struct CTransform
{
    Vec2  position = {0.f, 0.f},
          velocity = {0.f, 0.f};
    float angle    = 0.f,
          mass     = 2.f;

    CTransform(Vec2 p, Vec2 v, float a)
        : position(p), velocity(v), angle(a) {}
};

struct CShape
{
    sf::CircleShape circle;

    CShape(float radius, int points, const sf::Color& fill,
           const sf::Color& outline, float thickness)
        : circle(radius, points)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(radius, radius);
    }
};

struct CCollision
{
    float radius = 0.f;
    CCollision(float f) : radius(f) {}
};

struct CScore
{
    int score = 0;
    CScore(int s) : score(s) {}
};

struct CLifespan
{
    int remaining = 0,
        total     = 0;
    CLifespan(int total) : remaining(total), total(total) {}
};

struct CInput
{
    bool up    = false,
         down  = false,
         left  = false,
         right = false,
         shoot = false,
         super = false,
         pause = false;

    CInput() {}
};
