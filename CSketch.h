#pragma once
#include "CBase4618.h"
#include "cvui.h"
#include <string>

class CSketch : public CBase4618
{
private:
    cv::Size _size;

    int _colour = 0;
    bool _reset = false;

    cv::Point _raw_joystick;
    cv::Point _pos_cur;
    cv::Point _pos_prev;

    cv::Mat gen_UI();

public:
    CSketch(cv::Size size_canvas, int comm_port);
    ~CSketch();

    void gpio();
    void update();
    bool draw(); //bool for exit function

};

