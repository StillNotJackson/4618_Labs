#pragma once
#include "CBase4618.h"
class CSketch :
    public CBase4618
{
private:
    cv::Size _size;

    int _colour = 0;
    bool _reset = false;

public:
    CSketch(cv::Size& size_canvase, int comm_port);
    ~CSketch();

    void gpio();
    void update();
    bool draw(); //bool for exit function

};

