#pragma once
#include "CBase4618.h"
#include <opencv2/opencv.hpp>

class CPong : public CBase4618
{
private:
    cv::Mat _canvas;

    // Ball Stuff
    cv::Point2f _ball_pos;
    cv::Point2f _ball_dir;
    int _ball_radius;
    int _ball_speed;

    // paddle & score
    int _paddle_player_y;
    int _paddle_comp_y;
    int _score_player;
    int _score_comp;

    // Timing for FPS
    double _last_tick;
    double _fps;

    void reset_game();

public:
    CPong();
    ~CPong();

    
    void gpio();
    void update();
    bool draw();
};

