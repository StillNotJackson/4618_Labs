#pragma once
#include "CBase4618.h"
#include <opencv2/opencv.hpp>

class CPong : public CBase4618
{
private:
    cv::Mat _canvas;
	cv::Size _size = cv::Size(1000, 800);

    std::mutex _mutex;
    bool _is_running;


    bool _reset = false;

    // Ball Stuff
    cv::Point2f _ball_pos;
    cv::Point2f _ball_dir;
    int _ball_radius;
    int _ball_speed;

    // paddle & score
    int _paddle_player_y;
    int _paddle_pc_y;
    int _score_player;
    int _score_pc;

    double _last_tick;
    double _fps;



public:
    CPong(int comm_port);
    ~CPong();

    void run() override;
    void update_thread();
    void update_gpio();

    void gpio();
    void update();
    bool draw();
    void reset();

	
};

