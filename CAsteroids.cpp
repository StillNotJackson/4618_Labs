// ===== CAsteroids.cpp (CHANGES) =====
#include "stdafx.h"
#include "CAsteroids.h"

#include <iostream>
#include <ctime>
#include <cmath>

using namespace cv;
using namespace std;

static const float PI_F = 3.1415926535f;

float get_random() {
    return ((float)rand() / (float)RAND_MAX);
}



CGameObject::CGameObject() {
    _last_tick = (double)getTickCount();
    _position = Point2f(0, 0);
    _velocity = Point2f(0, 0);
    _radius = 1;
    _lives = 1;
}

void CGameObject::move() {
    double current_tick = (double)getTickCount();
    double dt = (current_tick - _last_tick) / getTickFrequency();
    _last_tick = current_tick;

    _position.x += (float)(dt * _velocity.x);
    _position.y += (float)(dt * _velocity.y);
}

bool CGameObject::collide(CGameObject& obj) {
    double dist = norm(_position - obj.get_pos());
    return dist < (_radius + obj._radius);
}

bool CGameObject::collide_wall(Size board) {
    if (_position.x - _radius < 0 || _position.x + _radius > board.width ||
        _position.y - _radius < 0 || _position.y + _radius > board.height) {
        return true;
    }
    return false;
}

void CGameObject::hit() {
    _lives--;
}

void CGameObject::draw(Mat& im) {
    circle(im, _position, _radius, Scalar(255, 255, 255), -1);
}

// --- Derived Classes ---

CShip::CShip() {
    _radius = 20;
    _lives = 10;
    _position = Point2f(400, 300);
    _velocity = Point2f(0, 0);
}

void CShip::draw(cv::Mat& im) {
    circle(im, _position, _radius, Scalar(0, 255, 0), -1); // green
}

CAsteroid::CAsteroid(Size board_size) {
    _radius = 15 + (int)(get_random() * 25);

    // Spawning
    float x = _radius + get_random() * (board_size.width - 2.0f * _radius);
    float y = _radius + get_random() * (board_size.height - 2.0f * _radius);
    _position = Point2f(x, y);

    //Movement 
    float angle = get_random() * 2.0f * PI_F;
    float speed = 40.0f + get_random() * 140.0f; // px/sec
    _velocity = Point2f(cosf(angle) * speed, sinf(angle) * speed);

    _lives = 1;
}

void CAsteroid::draw(cv::Mat& im) {
    circle(im, _position, _radius, Scalar(180, 180, 180), -1); // gray
}

CMissile::CMissile(Point2f start_pos) {
    _radius = 3;
    _position = start_pos;
    _velocity = Point2f(0, -300);
    _lives = 1;
}

void CMissile::draw(cv::Mat& im) {
    circle(im, _position, _radius, Scalar(0, 0, 255), -1); // red
}



CAsteroidGame::CAsteroidGame(int port) {
    srand((unsigned)time(nullptr));

    _canvas = Mat::zeros(600, 800, CV_8UC3);
    _comm.init_com(port);

    _fire_pressed = false;
    _reset_pressed = false;

    reset_game();
}

CAsteroidGame::~CAsteroidGame() {}

void CAsteroidGame::spawn_wave(int count) {
    for (int i = 0; i < count; i++) {
        CAsteroid a(_canvas.size());

        // Avoid spawning right on the ship
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

void CAsteroidGame::update() {
    
    float joy_x_pct = _comm.get_analog(JOYSTICK_X);
    float joy_y_pct = _comm.get_analog(JOYSTICK_Y);

    
    bool fire_press = _comm.get_button(BUTTON_FIRE);
    bool reset_press = _comm.get_button(BUTTON_RESET);

    
    if (joy_x_pct >= 0.0f && joy_y_pct >= 0.0f) {
        
        float nx = (joy_x_pct - 50.0f) / 50.0f;
        float ny = (joy_y_pct - 50.0f) / 50.0f;
        if (nx < -1) nx = -1; if (nx > 1) nx = 1;
        if (ny < -1) ny = -1; if (ny > 1) ny = 1;

        // Ghosting prevention
        const float DEAD = 0.10f;
        if (fabsf(nx) < DEAD) nx = 0.0f;
        if (fabsf(ny) < DEAD) ny = 0.0f;

        // y usually feels inverted on screen; flip if needed
        _ship.set_velocity(Point2f(-nx * SHIP_SPEED, ny * SHIP_SPEED));
    }

    if (fire_press) {
        _missile_list.push_back(CMissile(_ship.get_pos()));
    }


    if (reset_press) {
        reset_game();
        return;
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

    // NEW WAVE
    if (_asteroid_list.empty()) {
        spawn_wave(MAX_ASTEROIDS);
    }
}

bool CAsteroidGame::draw() {
    _canvas = Mat::zeros(_canvas.size(), CV_8UC3);

    if (_ship.get_lives() > 0) _ship.draw(_canvas);
    for (auto& a : _asteroid_list) a.draw(_canvas);
    for (auto& m : _missile_list) m.draw(_canvas);

    string hud = "Score: " + to_string(_score) +
        " | Lives: " + to_string(_ship.get_lives()) +
        " | Missiles: " + to_string(_missile_list.size()) +
        " | Asteroids: " + to_string(_asteroid_list.size());

    putText(_canvas, hud, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 2);
    imshow("Asteroids", _canvas);
    return true;
}

void CAsteroidGame::gpio() {
    (void)_comm.get_analog(JOYSTICK_X);
    (void)_comm.get_analog(JOYSTICK_Y);
    (void)_comm.get_button(BUTTON_FIRE);
    (void)_comm.get_button(BUTTON_RESET);
}