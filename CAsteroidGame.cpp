#include "stdafx.h"
#include "CAsteroidGame.h"

#include <iostream>
#include <ctime>
#include <cmath>
#include "cvui.h"

#include "CGameObject.h"
#include "CShip.h"
#include "CAsteroid.h"
#include "CMissile.h"

using namespace cv;
using namespace std;




CAsteroidGame::CAsteroidGame(int port) {
    srand(time(nullptr));

    _canvas = Mat::zeros(600, 800, CV_8UC3);
    _background = Mat::zeros(600, 800, CV_8UC3);
    int num_stars = 150; // Adjust for more or fewer stars
    for (int i = 0; i < num_stars; i++) {
        int x = rand() % _background.cols;
        int y = rand() % _background.rows;
        // Generate a random brightness for a twinkling effect
        int brightness = rand() % 155 + 100;

        // Draw the star. A 1-pixel circle is perfect for a star.
        cv::circle(_background, cv::Point(x, y), 1, cv::Scalar(brightness, brightness, brightness), -1);
    }
    
    _comm.init_com(port);

    cv::namedWindow("Asteroids");
    cvui::init("Asteroids");

    _fire_pressed = false;
    _reset_pressed = false;

    _level = 1;

    reset_game();
}

CAsteroidGame::~CAsteroidGame() {}

void CAsteroidGame::spawn_wave(int count) {
    for (int i = 0; i < count; i++) {
        CAsteroid a(_canvas.size());

        // Anti-spawn kill
        for (int tries = 0; tries < 10; tries++) {
            if (norm(a.get_pos() - _ship.get_pos()) > 140.0f) break;
            a = CAsteroid(_canvas.size());
        }

        _asteroid_list.push_back(a);
    }
}

void CAsteroidGame::reset_game() {
    _ship = CShip();
    _asteroid_list.clear();
    _missile_list.clear();
    _score = 0;
	_game_over = false;

    _ship_vel = cv::Point2f(0.0f, 0.0f);
    _ship.set_velocity(_ship_vel);
    _last_tick = (double)cv::getTickCount();



    spawn_wave(MAX_ASTEROIDS);
}


void CAsteroidGame::wrap_object(CGameObject& obj) {
    Point2f p = obj.get_pos();
    const float w = (float)_canvas.cols;
    const float h = (float)_canvas.rows;

    
    if (p.x < 0)     p.x += w;
    if (p.x >= w)    p.x -= w;
    if (p.y < 0)     p.y += h;
    if (p.y >= h)    p.y -= h;

    obj.set_pos(p);
}

void CAsteroidGame::update() 
{
    if (_game_over)
        return;

    
    static float heading = 0.0f;
    _ship.set_velocity(_ship_vel);

    // Forces heading to line up with inertia
    float sp = sqrtf(_ship_vel.x * _ship_vel.x + _ship_vel.y * _ship_vel.y);
    if (sp > 5.0f)
        heading = atan2f(_ship_vel.y, _ship_vel.x);

    _ship.set_angle(heading);


    if (_reset_pressed)
    {
        reset_game();
        return;
    }

    //Burst cannon 
    static bool burst_active = false;
    static int  burst_left = 0;
    static int  next_shot_ms = 0;

    int now_ms = (int)(cv::getTickCount() * 1000.0 / cv::getTickFrequency());

    if (_fire_pressed && !burst_active)
    {
        burst_active = true;
        burst_left = 3;
        next_shot_ms = now_ms;
    }

    const int BURST_INTERVAL_MS = 80;

    if (burst_active && now_ms >= next_shot_ms)
    {
        const float NOSE_OFFSET = 28.0f;
        const float MISSILE_SPEED = 450.0f;

        cv::Point2f ship_pos = _ship.get_pos();

        // moving -> missile moving 
        cv::Point2f forward(cosf(heading), sinf(heading));

        cv::Point2f missile_pos = ship_pos + forward * NOSE_OFFSET;

        CMissile m(missile_pos);
        m.set_velocity(forward * MISSILE_SPEED + _ship_vel);

        _missile_list.push_back(m);

        burst_left--;
        next_shot_ms = now_ms + BURST_INTERVAL_MS;

        if (burst_left <= 0)
            burst_active = false;
    }


    // Ship movement
    _ship.move();
    wrap_object(_ship);

    // Asteroid movement
    for (auto& a : _asteroid_list) {
        a.move();
        wrap_object(a);
    }

    // Missile movement
    for (auto& m : _missile_list) {
        m.move();
        if (m.collide_wall(_canvas.size())) m.hit();
    }

    // COLLISIONS
    for (auto& a : _asteroid_list) {
        if (a.collide(_ship)) {
            a.hit();
            _ship.hit();
        }
    }
    for (auto& a : _asteroid_list) {
        for (auto& m : _missile_list) {
            if (a.get_lives() > 0 && m.get_lives() > 0 && a.collide(m)) {
                a.hit();
                m.hit();
                _score += 10;
            }
        }
    }

    //REMOVAL
    _asteroid_list.erase(remove_if(_asteroid_list.begin(), _asteroid_list.end(),
        [](CAsteroid& a) { return a.get_lives() <= 0; }), _asteroid_list.end());
    _missile_list.erase(remove_if(_missile_list.begin(), _missile_list.end(),
        [](CMissile& m) { return m.get_lives() <= 0; }), _missile_list.end());
    
    // game over check
    if (_ship.get_lives() <= 0)
        _game_over = true;

    // new wave
    if (_asteroid_list.empty()) 
    {
        _level++;
        spawn_wave(MAX_ASTEROIDS* (_level));
    }
}

bool CAsteroidGame::draw() 
{
    cvui::context("Asteroids");
    //_canvas = Mat::zeros(_canvas.size(), CV_8UC3);
    _background.copyTo(_canvas);

    if (_ship.get_lives() > 0) _ship.draw(_canvas);
    for (auto& a : _asteroid_list) a.draw(_canvas);
    for (auto& m : _missile_list) m.draw(_canvas);

    string hud = "Score: " + to_string(_score) +
        " | Lives: " + to_string(_ship.get_lives()) +
        " | Missiles: " + to_string(_missile_list.size()) +
        " | Asteroids: " + to_string(_asteroid_list.size());

    putText(_canvas, hud, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
    
    static bool exit_requested = false;
    if (_game_over)
    {
        bool restart_clicked = splash_screen(exit_requested);

        if (restart_clicked)
        {
            exit_requested = false;
            reset_game();
        }
        else if (exit_requested)
        {
            return false; // only exit if Exit was actually clicked
        }
    }


    cvui::update();
    imshow("Asteroids", _canvas);
    return true;
}

void CAsteroidGame::gpio()
{
    
    float joy_x_pct = _comm.get_analog(JOYSTICK_X);
    float joy_y_pct = _comm.get_analog(JOYSTICK_Y);
    bool fire_raw = _comm.get_button(BUTTON_FIRE);
    bool reset_raw = _comm.get_button(BUTTON_RESET);

    
    double now_tick = (double)cv::getTickCount();
    float dt = 0.0f;

    if (_last_tick != 0.0)
        dt = (float)((now_tick - _last_tick) / cv::getTickFrequency());

    _last_tick = now_tick;

    // clamp dt
    if (dt > 0.05f) dt = 0.05f;
    if (dt < 0.0f)  dt = 0.0f;

    static bool fire_prev = false;
    static bool reset_prev = false;

    // pulse true for ONE frame only
    _fire_pressed = (fire_raw && !fire_prev);
    _reset_pressed = (reset_raw && !reset_prev);

    fire_prev = fire_raw;
    reset_prev = reset_raw;

    if (joy_x_pct >= 0.0f && joy_y_pct >= 0.0f)
    {
        // map 0..100 -> -1..1 (with your inversion)
        float nx = -(joy_x_pct - 50.0f) / 50.0f;
        float ny = (joy_y_pct - 50.0f) / 50.0f;

        // clamp
        if (nx < -1.0f) nx = -1.0f; if (nx > 1.0f) nx = 1.0f;
        if (ny < -1.0f) ny = -1.0f; if (ny > 1.0f) ny = 1.0f;

        // deadzone
        const float DEAD = 0.10f;
        if (fabsf(nx) < DEAD) nx = 0.0f;
        if (fabsf(ny) < DEAD) ny = 0.0f;

        cv::Point2f input(nx, ny);

        // normalize diagonal
        float mag = sqrtf(input.x * input.x + input.y * input.y);
        if (mag > 1.0f)
        {
            input *= (1.0f / mag);
            mag = 1.0f;
        }

        // inertia/steering params
        const float MAX_SPEED = SHIP_SPEED;  // px/sec
        const float STEER_RATE = 7.0f;
        const float DRAG_RATE = 1.2f;

        // current velocity is the existing member
        cv::Point2f v = _ship_vel;

        // steer only if stick pushed
        if (mag > 0.0f)
        {
            cv::Point2f desired = input * MAX_SPEED;
            v += (desired - v) * (STEER_RATE * dt);
        }

        // drag always
        float drag = 1.0f - (DRAG_RATE * dt);
        if (drag < 0.0f) drag = 0.0f;
        v *= drag;

        // clamp speed
        float sp = sqrtf(v.x * v.x + v.y * v.y);
        if (sp > MAX_SPEED) v *= (MAX_SPEED / sp);

        // OUTPUT of gpio(): updated "input state"
        _ship_vel = v;
    }
    else
    {
        // if joystick read is invalid, still apply drag so it doesn't drift forever
        const float DRAG_RATE = 1.2f;
        float drag = 1.0f - (DRAG_RATE * dt);
        if (drag < 0.0f) drag = 0.0f;
        _ship_vel *= drag;
    }
}

bool CAsteroidGame::splash_screen(bool& exit_requested)
{
    // dark overlay
    cv::rectangle(_canvas, cv::Point(0, 0), cv::Point(_canvas.cols, _canvas.rows), cv::Scalar(0, 0, 0), -1);

    cvui::window(_canvas, _canvas.cols / 2 - 150, _canvas.rows / 2 - 120, 300, 240, "GAME OVER");

    cvui::text(_canvas, _canvas.cols / 2 - 60, _canvas.rows / 2 - 60, "Score: " + std::to_string(_score)
    );

    if (cvui::button(_canvas, _canvas.cols / 2 - 100, _canvas.rows / 2, 200, 30, "Restart"))
    {
        return true;
    } 

    if (cvui::button(_canvas, _canvas.cols / 2 - 100, _canvas.rows / 2 + 50, 200, 30, "Exit"))
    {
        exit_requested = true;
    }
    return false;
}