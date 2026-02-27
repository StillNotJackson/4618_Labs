#include "stdafx.h"
#include "CPong.h"
#include "cvui.h"

#define CHAN_JOY_VER 4
#define CHAN_JOY_HOR 11

#define CHAN_S1 33
#define CHAN_S2 32

#define WINDOW_NAME "Pong"

CPong::CPong(int comm_port)
{

    _canvas = cv::Mat::zeros(_size, CV_8UC3);

    _comm.init_com(comm_port);

    cv::namedWindow(WINDOW_NAME);
    cvui::init(WINDOW_NAME);

    _ball_radius = 50;
    _ball_speed = 200;

    _last_tick = (double)cv::getTickCount();

    reset();
}

CPong::~CPong()
{
}



void CPong::gpio()
{
    

    static int last_ms = 0;
    int now_ms = (int)(cv::getTickCount() * 1000 / cv::getTickFrequency());

    if (now_ms - last_ms < 10) return; // 100 Hz polling
    last_ms = now_ms;


    _paddle_player_y = (int)((_comm.get_analog(CHAN_JOY_VER)/100.f) * _size.height);

    //Player collision with wall
    const int paddle_half = 50;
    if (_paddle_player_y < paddle_half)
    {
        _paddle_player_y += paddle_half;
    }
    if (_paddle_player_y > (_size.height - paddle_half))
    {
        _paddle_player_y = _size.height - paddle_half;
    }

    // PC collision with wall
     if (_paddle_pc_y < paddle_half)
    {
        _paddle_pc_y += paddle_half;
	 }
     if (_paddle_pc_y > (_size.height - paddle_half))
    {
        _paddle_pc_y = _size.height - paddle_half;
	 }


    if (_comm.get_button(CHAN_S1))
    {
        _reset = true;
    }
}

void CPong::update()
{
    // Timing calculation for smooth movement
    double current_tick = (double)cv::getTickCount();
    double delta_t = (current_tick - _last_tick) / cv::getTickFrequency();
    _last_tick = current_tick;
    _fps = 1.0 / delta_t;

    // Move ball
    _ball_pos += _ball_dir * (float)(_ball_speed * delta_t);

    //comp paddle
	_paddle_pc_y = (int)_ball_pos.y;

    // Wall Collision
    //if ((_ball_pos.y + _ball_radius) < _ball_radius || (_ball_pos.y+_ball_radius) > _size.height)// - _ball_radius)
    if (_ball_pos.y < _ball_radius || (_ball_pos.y + _ball_radius) > _size.height)// - _ball_radius)
    {
        _ball_dir.y *= -1;
    }

    // Paddle Collision (player)
    // Check if ball x is near paddle and y is within paddle height
    if (_ball_pos.x > 950 - _ball_radius && abs(_ball_pos.y - _paddle_player_y) < 60) // 60 is half paddle height + buffer
    {
        _ball_dir.x *= -1;
        _ball_pos.x = 950 - _ball_radius;
    }

    if (_ball_pos.x < 30 + _ball_radius && abs(_ball_pos.y - _paddle_pc_y) < 60) // Left paddle
    {
        _ball_dir.x *= -1;
        _ball_pos.x = 30 + _ball_radius;
	}

    // Scoring 
    if (_ball_pos.x < _ball_radius) {
        _score_player++;
        _ball_pos = cv::Point2f(_size.width / 2, _size.height / 2); // Reset to middle
    }
    if (_ball_pos.x > (1000 - _ball_radius)) {
        _score_pc++;
        _ball_pos = cv::Point2f(_size.width / 2, _size.height / 2);
    }
}

bool CPong::draw()
{
    _canvas.setTo(cv::Scalar(0, 0, 0));

    cv::Rect player_paddle(970, _paddle_player_y - 50, 20, 100);
    cv::rectangle(_canvas, player_paddle, cv::Scalar(255, 255, 255), cv::FILLED);

    cv::Rect pc_paddle(10, _paddle_pc_y - 50, 20, 100);
    cv::rectangle(_canvas, pc_paddle, cv::Scalar(255, 255, 255), cv::FILLED);

    cv::circle(_canvas, _ball_pos, _ball_radius, cv::Scalar(255, 255, 255), cv::FILLED);

    cvui::window(_canvas, 10, 10, 220, 250, "Pong Menu");

    cvui::printf(_canvas, 15, 35, "FPS: %.2f", _fps);
    cvui::printf(_canvas, 15, 55, "Player: %d   Computer: %d", _score_player, _score_pc);

    cvui::printf(_canvas, 15, 80, "Radius");
    cvui::trackbar(_canvas, 15, 95, 180, &_ball_radius, 5, 100);

    cvui::printf(_canvas, 15, 145, "Speed");
    cvui::trackbar(_canvas, 15, 160, 180, &_ball_speed, 100, 400);


    if (cvui::button(_canvas, 120, 210, "Exit"))
    {
        return false;
    }
    if (cvui::button(_canvas, 20, 210, "Reset"))
    {
        _reset = true;
    }

    if (_reset)
    {
        reset();
        _reset = false;
    }

    if (_score_pc >= 5 || _score_player >= 5)
    {
        cvui::printf(_canvas, _size.width / 2, _size.height / 2, "Game Over! %s wins!", _score_player > _score_pc ? "Player" : "Computer");
        reset();
	}
	cv::waitKey(1);


    cvui::update();
    cv::imshow(WINDOW_NAME, _canvas);
    return true;
}

void CPong::run()
{
    _is_running = true;

	//GPIO multi-threading
    std::thread t1(&CPong::update_gpio, this);
    t1.detach(); 

    while (_is_running)
    {
        update();

        if (!draw())
        {
            _is_running = false;
        }


        if (cv::waitKey(10) == 'q') // <-------------- This fucking thing controls the fps somehow
        {
            _is_running = false;
        }
    }
    cv::destroyAllWindows();
}


void CPong::update_thread()
{
    while (_is_running)
    {
        _mutex.lock();
        gpio();
        _mutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void CPong::update_gpio()
{
    while (_is_running)
    {
        _mutex.lock();
        gpio();
        _mutex.unlock();
        

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void CPong::reset()
{
    _ball_pos = cv::Point2f(_size.width / 2, _size.height / 2);
    _ball_dir = cv::Point2f(1.0f, 0.5f);

    _score_player = 0;
    _score_pc = 0;
    _paddle_player_y = _size.height / 2;
    _paddle_pc_y = _size.height / 2;
}

