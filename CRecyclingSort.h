#pragma once

#include "CBase4618.h"
#include <opencv2/core.hpp>
#include <string>
#include <vector>
#include "cvui.h"

class CRecyclingSort : public CBase4618
{
private:
    CServer _server;
    bool _system_on = false;

    //Camera stuff
    cv::VideoCapture _video;
    cv::Mat _frame;
    cv::Mat _hsv_frame;
    cv::Mat _gray_frame;
    cv::Mat _mask;
    cv::Rect _target_box;

    int _hue_low = 0;
    int _hue_high = 179;

    //tracking
    std::string _current_color = "NONE";
    std::string _current_bin = "NO OBJECT";
    std::string _last_print_status = "";
    double _target_distance = 1e9;

    int _bin1_count = 0;
    int _bin2_count = 0;
    bool _auto_mode = true;

    // servos
    bool _calibration_mode = false;
    int _cal_gate_open = 2000;
    int _cal_gate_close = 2400;
    int _cal_sort_bin1 = 1000;
    int _cal_sort_bin2 = 800;

public:
    CRecyclingSort();
    ~CRecyclingSort();

    void gpio() override;
    void update() override;
    bool draw() override;
};

