#include "stdafx.h"
#include "CSketch.h"
#include <opencv2/core.hpp>
//#include "cvui.h"

#define CHAN_LED_RED 39
#define CHAN_LED_GRN 38
#define CHAN_LED_BLU 37

#define CHAN_JOY_VER 4
#define CHAN_JOY_HOR 11

#define CHAN_S1 33
#define CHAN_S2 32

CSketch::CSketch(cv::Size size_canvas, int comm_port)
{
	_size = size_canvas;
	_comm.init_com(comm_port);

	_canvas = cv::Mat::zeros(_size, CV_8UC3);//8 unsigned bits with 3 channels
	cvui::init("Etch-A-Sketch");//Lab 4 - Embedded Systems
}

CSketch::~CSketch()
{

}

void CSketch::gpio()
{
	_pos_cur = cv::Point((int)((_comm.get_analog(CHAN_JOY_HOR) / 100.0f) * (_size.width - 1)), (int)((_comm.get_analog(CHAN_JOY_VER) / 100.0f) * (_size.height - 1)));
	
	if (_comm.get_button(CHAN_S1))
	{
		_reset = true;
	}
	//if (_comm.get_button(CHAN_S2)//colour button
	//TODO Colour change on button press based off _colour value
}
void CSketch::update()
{
	//_pos_prev = _pos_cur;

	//if (_comm.get_button(CHAN_S2)//colour button
}
bool CSketch::draw()
{
	// Praise this man https://fernandobevilacqua.com/cvui/components/button/
	cvui::context("Etch-A-Sketch");
	if (cvui::button(_canvas, 100, 45, 80, 30, "Exit"))
	{
		return false;//kills loop
	}
	if (cvui::button(_canvas, 10, 45, 80, 30, "Reset"))
	{
		_reset = true;
	}
	if (_reset)
	{
		_canvas = cv::Mat::zeros(_size, CV_8UC3);
		_reset = false;
	}
	cv::line(_canvas, _pos_prev, _pos_cur, _colour, 2);//Might need a funciton for _colour
	cv::imshow("Etch-A-Sketch", _canvas);
	return true; //return true continues loop
}