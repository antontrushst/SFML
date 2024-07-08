#include <iostream>
#include <vector>
#include <cmath>
#include "SFML\Graphics.hpp"
#include "vec2.h"             //////////   custom class for vector2   //////////

Vec2 win = { 800, 600 };
sf::RenderWindow window(sf::VideoMode(win.x, win.y), "collision");
sf::CircleShape  container(250, 128);

struct Object
{
    size_t id     = 0;
    Vec2  pos     = {0, 0},
          pos_old = {0, 0},
          acc     = {0, 0};
    float mass    = 0.f,
          radius  = 20.f;
    sf::CircleShape shape;

    Object(size_t nid)
        : id(nid), pos(300,win.y/2), pos_old(pos)
        , acc(0.f, 0.f), mass(3.f)
    {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(pos.x, pos.y);
        shape.setFillColor(sf::Color::Black);
    }
    Object(size_t nid, Vec2 npos)
        : id(nid), pos(npos), pos_old(pos)
        , acc(0.f, 0.f), mass(3.f)
    {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setPosition(pos.x, pos.y);
        shape.setFillColor(sf::Color::Black);
    }
    ~Object() {}

    void updatePosition(float dt)
    {
        Vec2 vel = pos - pos_old;
        pos_old = pos;
        pos = pos + vel + acc * dt * dt;
        acc = {};
        this->shape.setPosition(pos.x, pos.y);
    }

    void accelerate(Vec2 accel)
    {
        acc += accel;
    }
};

class Objects
{
    std::vector<Object> objVec,
                        to_Add;
    size_t              totalObjects;

public:
     Objects() {}
    ~Objects() {}

    void update()
    {
        for (auto o : to_Add)
            objVec.push_back(o);
        to_Add.clear();
    }

    void addObj()
    {
        Object o(totalObjects++);
        to_Add.push_back(o);
    }

    void addObj(Vec2 pos)
    {
        Object o(totalObjects++, pos);
        to_Add.push_back(o);
    }

    std::vector<Object>& getObjects()
    {
        return objVec;
    }
};

Objects objects;

//////////   SYSTEMS DECLARATIONS : start
void inputs         ();
void collision      ();
void render         ();
void update         (float);
void applyGravity   ();
void updatePositions(float);
//////////   SYSTEMS DECLARATIONS : end

////////////////////////////////////////////////////////////////////////////////
//////////                         MAIN : start                       //////////
int main()
{
    sf::Clock clock;
    sf::Time currentFrame = sf::seconds(0.f);
    window.setFramerateLimit(60);
    container.setFillColor(sf::Color::White);
    container.setOrigin(container.getRadius(), container.getRadius());
    container.setPosition(window.getSize().x/2, window.getSize().y/2);
   
    while (window.isOpen())
    {
        //////////   Delta Time calculation                           //////////
        sf::Time previousFrame = currentFrame;
        currentFrame = clock.getElapsedTime();
        float dt = currentFrame.asSeconds() - previousFrame.asSeconds();
        //////////                                                    //////////

        inputs();
        objects.update();
        applyGravity();
        update(dt);
        render();
    }

    return 0;
}
//////////                         MAIN : end                         //////////
////////////////////////////////////////////////////////////////////////////////

//////////   SYSTEMS DEFINITIONS : start
void inputs()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::O: objects.addObj(); break;
                default: break;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            switch (event.key.code)
            {
                case sf::Mouse::Left: Vec2 m_pos;
                                      m_pos = sf::Mouse::getPosition(window);
                                      objects.addObj(m_pos);
                                      break;
            }
        }
    }
}

void update(float dt)
{
    const uint32_t sub_steps = 8;
    const float    sub_dt    = dt / (float)sub_steps;
    for (uint32_t i(sub_steps); i--;)
    {
        applyGravity();
        collision();
        updatePositions(dt);
    }
}

void updatePositions(float dt)
{
    for (auto& o : objects.getObjects())
        o.updatePosition(dt);
}

void applyGravity()
{
    Vec2 gravity = { 0.f, 10.f };
    for (auto& o : objects.getObjects())
        o.accelerate(gravity);
}

void collision()
{
    //////////              Collision with the container              //////////
    for (auto& o1 : objects.getObjects())
    {
        Vec2 con_pos;
        con_pos = container.getPosition();
        const float con_rad = container.getRadius();
        const Vec2 to_obj  = o1.pos - container.getPosition();
        const float dist   = sqrtf( to_obj.x * to_obj.x + to_obj.y * to_obj.y );

        if (dist > con_rad - o1.shape.getRadius())
        {
            const Vec2 n = to_obj / dist;
            o1.pos = con_pos + n * (con_rad - o1.shape.getRadius());
        }
    }
    //////////                                                        //////////

    //////////               Collision between objects                //////////
    const uint32_t object_count = objects.getObjects().size();
    for (uint32_t i(0); i < object_count; ++i)
    {
        Object& object_1 = objects.getObjects()[i];
        for (uint32_t k(i+1); k < object_count; ++k)
        {
            Object& object_2 = objects.getObjects()[k];
            const float obj_1r = object_1.shape.getRadius(),
                        obj_2r = object_2.shape.getRadius(),
                        r2     = obj_1r + obj_2r;
            const Vec2 collision_axis = object_1.pos - object_2.pos;
            const float dist = sqrtf( collision_axis.x * collision_axis.x +
                                      collision_axis.y * collision_axis.y);
            if (dist < r2)
            {
                const Vec2 n = collision_axis / dist;
                const float delta = r2 - dist;
                object_1.pos += n * (.5f * delta);
                object_2.pos -= n * (.5f * delta);
            }
        }
    }
    //////////                                                        //////////
    
}

void render()
{
    window.clear(sf::Color::Black);
    window.draw(container);
    for (auto o : objects.getObjects())
        window.draw(o.shape);
    window.display();
}
//////////   SYSTEMS DEFINITIONS : end
