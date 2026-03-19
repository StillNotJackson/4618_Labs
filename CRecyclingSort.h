#pragma once

#include "CBase4618.h"
#include <opencv2/core.hpp>
#include "cvui.h"

class CRecyclingSort : public CBase4618
{
private:
	
	int _hue_low = 0;
	int _hue_high = 255;

	cv::VideoCapture _video;
	cv::Mat _frame;
	cv::Mat _hsv_frame;
	cv::Mat _mask;

public:
	CRecyclingSort();
	~CRecyclingSort();

	void gpio() override;
	void update() override;
	bool draw() override;
};

