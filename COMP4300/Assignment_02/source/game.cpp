#include "game.h"
#include "genFloat.h"
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

Game::Game(const std::string& config) { init(config); }

void Game::init(const std::string& config)
{
    //   get the map of keywords to read config.txt file   .....................
    enum KeyWord { Window, Font, Player, Enemy, Bullet };
    std::map<std::string, KeyWord> mp;
    mp["Window"] = KeyWord::Window;
    mp["Font"]   = KeyWord::Font;
    mp["Player"] = KeyWord::Player;
    mp["Enemy"]  = KeyWord::Enemy;
    mp["Bullet"] = KeyWord::Bullet;

    //   exit with error if can't open config.txt file   .......................
    std::ifstream fin("config.txt");
    if (!fin.is_open())
    {
        std::cerr << "Couldn't open \"config.txt\" file!\n";
        exit(1);
    }

    //   read config.txt file   ................................................
    std::string temp;
    while (fin >> temp)
    {
        std::map<std::string, KeyWord>::iterator it;
        it = mp.find(temp);
        switch (it->second)
        {
            case KeyWord::Window:
                fin >> m_windowConfig.WindowWidth >> m_windowConfig.WindowHeight
                    >> m_windowConfig.FrameLimit  >> m_windowConfig.FullScreen;
                break;
            case KeyWord::Font:
                fin >> m_fontConfig.Path   >> m_fontConfig.Size
                    >> m_fontConfig.ColorR >> m_fontConfig.ColorG
                    >> m_fontConfig.ColorB;
                break;
            case KeyWord::Player:
                fin >> m_playerConfig.SR >> m_playerConfig.CR
                    >> m_playerConfig.FR >> m_playerConfig.FG
                    >> m_playerConfig.FB >> m_playerConfig.OR
                    >> m_playerConfig.OG >> m_playerConfig.OB
                    >> m_playerConfig.OT >> m_playerConfig.V
                    >> m_playerConfig.S;
                break;
            case KeyWord::Enemy:
                fin >> m_enemyConfig.SR   >> m_enemyConfig.CR
                    >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
                    >> m_enemyConfig.OR   >> m_enemyConfig.OG
                    >> m_enemyConfig.OB   >> m_enemyConfig.OT
                    >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX
                    >> m_enemyConfig.L    >> m_enemyConfig.SI;
                break;
            case KeyWord::Bullet:
                fin >> m_bulletConfig.SR >> m_bulletConfig.CR
                    >> m_bulletConfig.S >> m_bulletConfig.FR
                    >> m_bulletConfig.FG >> m_bulletConfig.FB
                    >> m_bulletConfig.OR >> m_bulletConfig.OG
                    >> m_bulletConfig.OB >> m_bulletConfig.OT
                    >> m_bulletConfig.V  >> m_bulletConfig.L;
                break;
            default: continue;
        }
    }

    // create a window   .......................................................
    if (m_windowConfig.FullScreen)
    {
        m_window.create(sf::VideoMode(m_windowConfig.WindowWidth,
                                    m_windowConfig.WindowHeight), 
                                    "Assignment 2", sf::Style::Fullscreen);
    }
    else
    {
        m_window.create(sf::VideoMode(m_windowConfig.WindowWidth,
                                    m_windowConfig.WindowHeight), 
                                    "Assignment 2");
    }
    m_window.setFramerateLimit(m_windowConfig.FrameLimit);

    //   create score and player   .............................................
    setScore();
    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        //   update entities   .................................................
        m_entities.update();

        //   spawn player if there's none   ....................................
        if (m_entities.getEntities("player").size() == 0)
            spawnPlayer();

        //   execute this systems only when the game is not paused   ...........
        if (!m_paused)
        {
            sEnemySpawner();
            sMovement();
            sLifespan();
            sCollision();
        }

        //   Input and Render systems   ........................................
        sUserInput();
        sRender();

        //   frame count   .....................................................
        m_currentFrame++;
    }
    
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::setScore()
{
    //   exit with an error if font didn't load   ..............................
    if (!m_font.loadFromFile(m_fontConfig.Path))
    {
        std::cerr << "Couldn't load font!\n";
        exit(1);
    }

    //   set font properties   .................................................
    m_text.setFont(m_font);
    m_text.setString("Score: " + std::to_string(m_score));
    m_text.setCharacterSize(m_fontConfig.Size);
    m_text.setFillColor(sf::Color(m_fontConfig.ColorR,
                                  m_fontConfig.ColorG,
                                  m_fontConfig.ColorB));
    auto bounds = m_text.getGlobalBounds();
    m_text.setOrigin(bounds.width/2, bounds.height/2);
    m_text.setPosition(bounds.width/2 + 20, 20);
}

void Game::spawnPlayer()
{
    //   create the player   ...................................................
    auto entity = m_entities.addEntity("player");

    //   get the middle of the screen   ........................................
    float mid_x = m_window.getSize().x / 2.f,
          mid_y = m_window.getSize().y / 2.f;

    //   set player properties   ...............................................
    entity->cTransform = std::make_shared<CTransform>(Vec2(mid_x, mid_y),
                                                      Vec2(0, 0), 0.f);
    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR,
                                              m_playerConfig.V,
                                              sf::Color(m_playerConfig.FR,
                                                        m_playerConfig.FG,
                                                        m_playerConfig.FB),
                                              sf::Color(m_playerConfig.OR,
                                                        m_playerConfig.OG,
                                                        m_playerConfig.OB),
                                              m_playerConfig.OT);
    entity->cShape->circle.setPosition(mid_x, mid_y);
    entity->cInput = std::make_shared<CInput>();
    //   assign the created player to m_player variable   ......................
    m_player = entity;
}

void Game::spawnEnemy()
{
    //   update m_lastEnemySpawnTime and create an enemy   .....................
    m_lastEnemySpawnTime = m_currentFrame;
    auto entity = m_entities.addEntity("enemy");

    //   get the area around player   ..........................................
    float player_rad = m_player->cShape->circle.getRadius();
    Vec2 player_pos = m_player->cShape->circle.getPosition(),
         min_dist   = player_pos - (player_rad + 100.f),
         max_dist   = player_pos + (player_rad + 100.f);

    //   setup randomizers   ...................................................
    std::random_device rd;
    std::mt19937 gen(rd());
    typedef std::uniform_int_distribution<> int_distrib;
    Vec2 win_size = {(float)m_window.getSize().x, (float)m_window.getSize().y};
    int_distrib distribX(m_enemyConfig.CR, win_size.x - m_enemyConfig.CR);
    int_distrib distribY(m_enemyConfig.CR, win_size.y - m_enemyConfig.CR);
    int_distrib distribV(m_enemyConfig.VMIN, m_enemyConfig.VMAX);

    //   generate random position outside player's area   ......................
    float randPosX = 0.f, randPosY = 0.f;
    while (randPosX > min_dist.x && randPosX < max_dist.x &&
           randPosY > min_dist.y && randPosY < max_dist.y ||
           randPosX == 0.f && randPosY == 0.f)
    {
        randPosX = (float)distribX(gen);
        randPosY = (float)distribY(gen);
    }

    //   generate random speed   ...............................................
    float randSpdX = genFloat(m_enemyConfig.SMIN, m_enemyConfig.SMAX),
          randSpdY = genFloat(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
    //   generate random number of vertecies   .................................
    int   randVert = distribV(gen);

    //   set enemy properties   ................................................
    entity->cTransform = std::make_shared<CTransform>(Vec2(randPosX, randPosY),
                                                      Vec2(randSpdX, randSpdY),
                                                      0.f);
    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, randVert,
                                              sf::Color(m_enemyConfig.OR - 150,
                                                        m_enemyConfig.OG,
                                                        m_enemyConfig.OB),
                                              sf::Color(m_enemyConfig.OR,
                                                        m_enemyConfig.OG,
                                                        m_enemyConfig.OB),
                                              m_enemyConfig.OT);
    entity->cShape->circle.setPosition(entity->cTransform->position.x,
                                            entity->cTransform->position.y);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    //   get enemy position, vertecies and colors   ............................
    Vec2 pos = e->cShape->circle.getPosition();
    int num_verts = e->cShape->circle.getPointCount(),
        radius    = e->cShape->circle.getRadius();
    sf::Color fill_color = e->cShape->circle.getFillColor(),
              outl_color = e->cShape->circle.getOutlineColor();

    //   create s.enemy (particle) for each vertex   ...........................
    for (int v = 0; v < e->cShape->circle.getPointCount(); v++)
    {
        Vec2 p_pos = e->cShape->circle.getTransform().transformPoint(e->cShape->
                                                            circle.getPoint(v));
        Vec2 diff = p_pos - pos;
        float dist = sqrtf(diff.x*diff.x + diff.y*diff.y);
        Vec2 norm = {diff.x/dist, diff.y/dist};
        auto small = m_entities.addEntity("small_enemy");
        small->cTransform = std::make_shared<CTransform>(pos, norm, 0.f);
        small->cShape     = std::make_shared<CShape>(radius/3, num_verts,
                                                     fill_color, outl_color, 1);
        small->cLifespan  = std::make_shared<CLifespan>(60);
    }
}

void Game::sRender()
{
    //   clear the window   ....................................................
    m_window.clear();

    //   set score text   ......................................................
    m_text.setString("Score: " + std::to_string(m_score));

    //   draw each entity   ....................................................
    for (auto& e : m_entities.getEntities())
    {
        e->cTransform->angle += 1.f;
        e->cShape->circle.setRotation(e->cTransform->angle);
        m_window.draw(e->cShape->circle);
    }

    // draw the score and display the window   .................................
    m_window.draw(m_text);
    m_window.display();
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        //   Escape (close the game)   .........................................
        if (event.type == sf::Event::Closed     ||
            event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape)
            m_running = false;

        //   P (pause the game)   ..............................................
        if (event.type == sf::Event::KeyPressed    &&
            event.key.code == sf::Keyboard::P &&
            m_player->cInput->pause == false)
        {
            m_player->cInput->pause = true;
            if (!m_paused)
                setPaused(true);
            else
                setPaused(false);
        }
        else if (event.type == sf::Event::KeyReleased   &&
                 event.key.code == sf::Keyboard::P &&
                 m_player->cInput->pause == true)
        {
            m_player->cInput->pause = false;
        }

        //   W, A, S, D (move player)   ........................................
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W: m_player->cInput->up    = true; continue;
                case sf::Keyboard::S: m_player->cInput->down  = true; continue;
                case sf::Keyboard::A: m_player->cInput->left  = true; continue;
                case sf::Keyboard::D: m_player->cInput->right = true; continue;
                default: break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W: m_player->cInput->up    = false; continue;
                case sf::Keyboard::S: m_player->cInput->down  = false; continue;
                case sf::Keyboard::A: m_player->cInput->left  = false; continue;
                case sf::Keyboard::D: m_player->cInput->right = false; continue;
                default: break;
            }
        }

        //   Left/Right Mouse Buttons (shoot/special weapon)   .................
        if (event.type == sf::Event::MouseButtonPressed)
        {
            switch (event.key.code)
            {
                case sf::Mouse::Left:  spawnBullet(); break;
                case sf::Mouse::Right: spawnSpecialWeapon(); break;
            }
        }
    }
}

void Game::sMovement()
{
    //   PLAYER MOVEMENT   /////////////////////////////////////////////////////
    m_player->cTransform->velocity = Vec2{ 0.f, 0.f };
    float player_radius   = m_player->cShape->circle.getRadius();
    Vec2  player_position = m_player->cShape->circle.getPosition();

    //   constrain player's movement inside the window frame ...................
    if (m_player->cInput->up    && !m_player->cInput->down
                                && player_position.y > player_radius)
        m_player->cTransform->velocity.y = m_playerConfig.S * -1;
    if (m_player->cInput->down  && !m_player->cInput->up
                                && player_position.y + player_radius <
                                   m_window.getSize().y)
        m_player->cTransform->velocity.y = m_playerConfig.S;
    if (m_player->cInput->left  && !m_player->cInput->right
                                && player_position.x > player_radius)
        m_player->cTransform->velocity.x = m_playerConfig.S * -1;
    if (m_player->cInput->right && !m_player->cInput->left
                                && player_position.x + player_radius <
                                   m_window.getSize().x)
        m_player->cTransform->velocity.x = m_playerConfig.S;

    //   move the player   .....................................................
    m_player->cTransform->position.x += m_player->cTransform->velocity.x;
    m_player->cTransform->position.y += m_player->cTransform->velocity.y;
    m_player->cShape->circle.setPosition(m_player->cTransform->position.x,
                                         m_player->cTransform->position.y);

    //   ENEMY MOVEMENT   //////////////////////////////////////////////////////
    for (auto& enemy : m_entities.getEntities("enemy"))
    {
        float enemy_radius   = enemy->cShape->circle.getRadius();
        Vec2  *enemy_position = &enemy->cTransform->position,
              *enemy_velocity = &enemy->cTransform->velocity;

        //   bounce from the window frame   ....................................
        if (enemy_position->x <= enemy_radius)
        {
            enemy_position->x = enemy_radius + 1;
            enemy_velocity->x *= -1;
        }
        else if (enemy_position->x >= m_window.getSize().x - enemy_radius)
        {
            enemy_position->x = m_window.getSize().x - (enemy_radius + 1);
            enemy_velocity->x *= -1;
        }
        else if (enemy_position->y <= enemy_radius)
        {
            enemy_position->y = enemy_radius + 1;
            enemy_velocity->y *= -1;
        }
        else if (enemy_position->y >= m_window.getSize().y - enemy_radius)
        {
            enemy_position->y = m_window.getSize().y - (enemy_radius + 1);
            enemy_velocity->y *= -1;
        }

        //   move the enemy   ..................................................
        enemy_position->x += enemy_velocity->x;
        enemy_position->y += enemy->cTransform->velocity.y;
        enemy->cShape->circle.setPosition(enemy_position->x, enemy_position->y);
    }

    //   BULLET MOVEMENT   /////////////////////////////////////////////////////
    for (auto& bullet : m_entities.getEntities("bullet"))
    {
        bullet->cTransform->position += bullet->cTransform->velocity;
        bullet->cShape->circle.setPosition(bullet->cTransform->position.x,
                                            bullet->cTransform->position.y);
    }

    //   SMALL ENEMIES (particles) MOVEMENT   //////////////////////////////////
    for (auto& s_enemy : m_entities.getEntities("small_enemy"))
    {
        s_enemy->cTransform->position += s_enemy->cTransform->velocity;
        s_enemy->cShape->circle.setPosition(s_enemy->cTransform->position.x,
                                            s_enemy->cTransform->position.y);
    }

    //   WAVE (special weapon) MOVEMENT   //////////////////////////////////////
    for (auto& wave : m_entities.getEntities("wave"))
    {
        float rad = wave->cShape->circle.getRadius();
        wave->cShape->circle.setOrigin(rad, rad);
        wave->cShape->circle.setRadius(rad += 10.f);
    }
}

void Game::sLifespan()
{
    for (auto& o : m_entities.getEntities())
    {
        //   skip if "player" or "enemy"   .....................................
        if (o->tag() == "player" || o->tag() == "enemy")
            continue;

        //   destroy object if its remaining time is 0   .......................
        if (o->cLifespan->remaining <= 0)
        {
            o->destroy();
            break;
        }

        //   get object's alphas and calculate the decrements   ................
        float alpha_fill      = (float)o->cShape->circle.getFillColor().a,
              alpha_outl      = (float)o->cShape->circle.getOutlineColor().a,
              decrement_fill  = alpha_fill / o->cLifespan->total,
              decrement_outl  = alpha_outl / o->cLifespan->total;


        //   get object's colors   .............................................
        int col_r = o->cShape->circle.getFillColor().r,
            col_g = o->cShape->circle.getFillColor().g,
            col_b = o->cShape->circle.getFillColor().b,
            out_r = o->cShape->circle.getOutlineColor().r,
            out_g = o->cShape->circle.getOutlineColor().g,
            out_b = o->cShape->circle.getOutlineColor().b;

        //   set same colors but new alphas   ..................................
        o->cShape->circle.setFillColor   (sf::Color(col_r, col_g, col_b,
                                                    alpha_fill-decrement_fill));
        o->cShape->circle.setOutlineColor(sf::Color(out_r, out_g, out_b,
                                                    alpha_outl-decrement_outl));

        //   decrease remaining time   .........................................
        o->cLifespan->remaining--;
    }
}

void Game::sEnemySpawner()
{
    if (m_currentFrame > m_lastEnemySpawnTime + m_enemyConfig.SI)
        spawnEnemy();
}

void Game::sCollision()
{
    for (auto& e : m_entities.getEntities("enemy"))
    {
        //   ENEMY-BULLET COLLISION   //////////////////////////////////////////
        for (auto& b : m_entities.getEntities("bullet"))
        {
            Vec2  e_pos = e->cShape->circle.getPosition(),
                  b_pos = b->cShape->circle.getPosition(),
                  diff  = e_pos - b_pos;
            float dist  = sqrtf( diff.x*diff.x + diff.y*diff.y ),
                  e_rad = e->cShape->circle.getRadius(),
                  b_rad = b->cShape->circle.getRadius();

            //   destroy enemy and bullet, increase the score   ................
            if (dist < (e_rad + b_rad))
            {
                spawnSmallEnemies(e);
                b->destroy();
                e->destroy();
                m_score += 100;
            }
        }
        
        //   ENEMY-PLAYER COLLISION   //////////////////////////////////////////
        for (auto& p : m_entities.getEntities("player"))
        {
            Vec2  e_pos = e->cShape->circle.getPosition(),
                  p_pos = p->cShape->circle.getPosition(),
                  diff  = e_pos - p_pos;
            float dist  = sqrtf( diff.x*diff.x + diff.y*diff.y ),
                  e_rad = e->cShape->circle.getRadius(),
                  p_rad = p->cShape->circle.getRadius();

            //   destroy the player, reset the score   .........................
            if (dist < (e_rad + p_rad))
            {
                p->destroy();
                m_score = 0;
            }
        }

        //   ENEMY-WAVE (special weapon) COLLISION   ///////////////////////////
        for (auto& w : m_entities.getEntities("wave"))
        {
            Vec2  e_pos = e->cShape->circle.getPosition(),
                  w_pos = w->cShape->circle.getPosition(),
                  diff  = e_pos - w_pos;
            float dist  = sqrtf( diff.x*diff.x + diff.y*diff.y ),
                  e_rad = e->cShape->circle.getRadius(),
                  w_rad = w->cShape->circle.getRadius();

            //   don't destroy enemies that are not on wave's circle line   ....
            if (dist + e_rad < w_rad)
                continue;

            //   destroy enemy, increase the score   ...........................
            if (dist - w_rad < e_rad)
            {
                spawnSmallEnemies(e);
                e->destroy();
                m_score += 100;
            }
        }
    }
}

void Game::spawnBullet()
{
    //   calculate and normilize bullet's vector   .............................
    Vec2 mouse_position  = sf::Mouse::getPosition(m_window),
         player_position = m_player->cShape->circle.getPosition(),
         diff_v          = mouse_position - player_position,
         norm_v,
         velo_v;
    float distance  = sqrtf (diff_v.x * diff_v.x + diff_v.y * diff_v.y);
    norm_v = Vec2(diff_v.x / distance, diff_v.y / distance);
    velo_v = Vec2(norm_v.x * m_bulletConfig.S, norm_v.y * m_bulletConfig.S);

    //   create a bullet   .....................................................
    auto bullet = m_entities.addEntity("bullet");
    bullet->cTransform = std::make_shared<CTransform>(player_position,
                                                      velo_v, 0);
    bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR,
                                              m_bulletConfig.V,
                                              sf::Color(m_bulletConfig.FR,
                                                        m_bulletConfig.FG,
                                                        m_bulletConfig.FB),
                                              sf::Color(m_bulletConfig.OR,
                                                        m_bulletConfig.OG,
                                                        m_bulletConfig.OB),
                                              m_bulletConfig.OT);
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon()
{
    //   don't spawn if an instance of sp. weapon already exists   .............
    if (m_entities.getEntities("wave").size() != 0)
        return;

    //   create the special weapon's wave   ....................................
    Vec2 player_pos = m_player->cShape->circle.getPosition();
    auto player_rad = m_player->cShape->circle.getRadius();
    auto wave = m_entities.addEntity("wave");
    wave->cTransform = std::make_shared<CTransform>(player_pos, Vec2(0,0), 0);
    wave->cShape     = std::make_shared<CShape>(player_rad, 30,
                                                sf::Color(255, 255, 255, 0),
                                                sf::Color(255, 255, 255), 5);
    wave->cShape->circle.setOrigin(wave->cShape->circle.getRadius(),
                                   wave->cShape->circle.getRadius());
    wave->cShape->circle.setPosition(player_pos.x, player_pos.y);
    wave->cLifespan = std::make_shared<CLifespan>(90);
}
