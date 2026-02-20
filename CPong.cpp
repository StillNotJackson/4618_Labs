#include "stdafx.h"
#include "CPong.h"
#include "cvui.h"

// Assuming joystick Y-axis is on analog channel 9 - adjust as needed for your E4618 setup
#define CHAN_JOY_VERT 9 

CPong::CPong()
{

    _canvas = cv::Mat::zeros(800, 1000, CV_8UC3);


    cv::namedWindow("Pong");
    cvui::init("Pong");

    _ball_radius = 50;
    _ball_speed = 200;

    _last_tick = (double)cv::getTickCount();
    reset_game();
}

CPong::~CPong()
{
}

void CPong::reset_game()
{
    //Ball reset
    _ball_pos = cv::Point2f(500, 400);
    _ball_dir = cv::Point2f(1.0f, 0.5f); 

    _score_player = 0;
    _score_comp = 0;
    _paddle_player_y = 400;
    _paddle_comp_y = 400;
}

void CPong::gpio()
{
    float joy_y_percent = _comm.get_analog(CHAN_JOY_VERT);

    if (joy_y_percent >= 0) {
        _paddle_player_y = (int)((joy_y_percent / 100.0f) * 800.0f);
    }
}

void CPong::update()
{
    // Calculate FPS
    double current_tick = (double)cv::getTickCount();
    double delta_t = (current_tick - _last_tick) / cv::getTickFrequency();
    _last_tick = current_tick;

    if (delta_t > 0) {
        _fps = 1.0 / delta_t;
    }

    // Move the ball based on current velocity and time since last drawn [cite: 29]
    _ball_pos.x += _ball_dir.x * _ball_speed * delta_t;
    _ball_pos.y += _ball_dir.y * _ball_speed * delta_t;

    // TODO Add collision here (walls & paddles) 
}

bool CPong::draw()
{
    _canvas = cv::Mat::zeros(800, 1000, CV_8UC3);


    cvui::window(_canvas, 10, 10, 220, 250, "Pong Menu");

 
    cvui::printf(_canvas, 15, 35, "FPS: %.2f", _fps);
    cvui::printf(_canvas, 15, 55, "Player: %d   Computer: %d", _score_player, _score_comp);

    cvui::printf(_canvas, 15, 80, "Radius");
    cvui::trackbar(_canvas, 15, 95, 180, &_ball_radius, 5, 100);


    cvui::printf(_canvas, 15, 145, "Speed");
    cvui::trackbar(_canvas, 15, 160, 180, &_ball_speed, 100, 400);


    if (cvui::button(_canvas, 20, 210, "Reset")) {
        reset_game();
    }
    if (cvui::button(_canvas, 120, 210, "Exit")) {
        return false; 
    }


    cv::Rect player_paddle(970, _paddle_player_y - 50, 20, 100);
    cv::rectangle(_canvas, player_paddle, cv::Scalar(255, 255, 255), cv::FILLED);

    cv::Rect comp_paddle(10, _paddle_comp_y - 50, 20, 100);
    cv::rectangle(_canvas, comp_paddle, cv::Scalar(255, 255, 255), cv::FILLED);

    cv::circle(_canvas, _ball_pos, _ball_radius, cv::Scalar(255, 255, 255), cv::FILLED);


    cvui::update();
    cv::imshow("Pong", _canvas);

    return true;
}