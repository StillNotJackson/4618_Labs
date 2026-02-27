#pragma once
#include "CBase4618.h"
#include <vector>
#include <opencv2/core.hpp>



class CGameObject {
protected:
    cv::Point2f _position;
    cv::Point2f _velocity;
    int _radius;
    int _lives;
    double _last_tick;

public:
    CGameObject();
    void move();
    bool collide(CGameObject& obj);
    bool collide_wall(cv::Size board);
    void hit();

    int get_lives() { return _lives; }
    void set_lives(int lives) { _lives = lives; }
    void set_pos(cv::Point2f pos) { _position = pos; }
    cv::Point2f get_pos() { return _position; }
    void set_velocity(cv::Point2f velocity) { _velocity = velocity; }

    virtual void draw(cv::Mat& im);
};

class CShip : public CGameObject {
public:
    CShip();
    void draw(cv::Mat& im) override;
};

class CAsteroid : public CGameObject {
public:
    CAsteroid(cv::Size board_size);
    void draw(cv::Mat& im) override;
};
    

class CMissile : public CGameObject {
public:
    CMissile(cv::Point2f start_pos);
    void draw(cv::Mat& im) override;
};


class CAsteroidGame : public CBase4618 {
private:
    CShip _ship;
    std::vector<CAsteroid> _asteroid_list;
    std::vector<CMissile> _missile_list;
    int _score;

    const int JOYSTICK_X = 11;
    const int JOYSTICK_Y = 4;
    const int BUTTON_FIRE = 32;
    const int BUTTON_RESET = 33;

    bool _fire_pressed;
    bool _reset_pressed;

    static const int MAX_ASTEROIDS = 10;
    void spawn_wave(int count);


    static constexpr float SHIP_SPEED = 250.0f; // pixels/sec
    void wrap_object(CGameObject& obj);//boundary reflection

    void reset_game();

public:
    CAsteroidGame(int port);
    ~CAsteroidGame();

    void gpio() override;
    void update() override;
    bool draw() override;
};