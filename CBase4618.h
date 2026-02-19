#pragma once

#include <opencv2/core.hpp>
#include "CControl.h"

class CBase4618
{
protected:
	CControl _comm;
	cv::Mat _canvas;

	bool _user_exit = false;

public:
	CBase4618();
	~CBase4618();

	virtual void gpio() = 0;
	virtual void update() = 0;
	virtual bool draw() = 0;

	void run();

};