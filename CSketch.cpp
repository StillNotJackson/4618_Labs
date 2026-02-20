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

#define WINDOW_NAME "Etch-A-Sketch"
#define CURSOR_SPEED 25

CSketch::CSketch(cv::Size size_canvas, int comm_port)
{
	_size = size_canvas;
	_comm.init_com(comm_port);

	_canvas = cv::Mat::zeros(_size, CV_8UC3);//8 unsigned bits with 3 channels
	cvui::init("Etch-A-Sketch");
	//cv::setMouseCallback(WINDOW_NAME, 

	//Cursor init
	_pos_cur = cv::Point(_size.width / 2, _size.height / 2); // Start Cursor in middle of window
	_pos_prev = _pos_cur;
}

CSketch::~CSketch()
{

}

void CSketch::gpio()
{
	_raw_joystick = cv::Point((int)((_comm.get_analog(CHAN_JOY_HOR) / 100.0f) * (_size.width - 1)), (int)(((100.0f - _comm.get_analog(CHAN_JOY_VER)) / 100.0f) * (_size.height - 1)));
	//_pos_cur = cv::Point((int)((_comm.get_analog(CHAN_JOY_HOR)/100.f) * (_size.width - 1)), (int)((_comm.get_analog(CHAN_JOY_VER)/100.0f) * (_size.height - 1)));

	if (_comm.get_button(CHAN_S1))
	{
		_reset = true;
	}
	//if (_comm.get_button(CHAN_S2)//colour button
	//TODO Colour change on button press based off _colour value
}
void CSketch::update()
{
	_pos_prev = _pos_cur;

	//if (_comm.get_button(CHAN_S2)//colour button

	int dx = _raw_joystick.x - (_size.width / 2);
	int dy = _raw_joystick.y - (_size.height / 2);

	//Joystick drift solution per kasra's rec
	const int DRIFT_ZONE = 20;
	if (dx > -(DRIFT_ZONE) && dx < DRIFT_ZONE)
		dx = 0;
	if (dy > -(DRIFT_ZONE) && dy < DRIFT_ZONE)
		dy = 0;

	dx /= CURSOR_SPEED;
	dy /= CURSOR_SPEED;

	_pos_cur.x += dx;
	_pos_cur.y += dy;

	//Border patrol of canvans
	if (_pos_cur.x < 0)
		_pos_cur.x = 0;
	if (_pos_cur.x > _size.width - 1)
		_pos_cur.x = _size.width - 1;

	if (_pos_cur.y < 80)
		_pos_cur.y = 80;
	if (_pos_cur.y > _size.height - 1)
		_pos_cur.y = _size.height - 1;


}
bool CSketch::draw()
{
	// Praise this man https://fernandobevilacqua.com/cvui/components/button/
	cvui::context(WINDOW_NAME);
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
		_pos_cur = cv::Point(_size.width / 2, _size.height / 2);
		_pos_prev = _pos_cur;

		_reset = false;
	}
	//cv::line(_canvas, _pos_prev, _pos_cur, _colour, 2);//Might need a funciton for _colour
	cv::line(_canvas, _pos_prev, _pos_cur, cv::Scalar(255, 255, 255), 2);
	cvui::update(WINDOW_NAME);
	cv::imshow(WINDOW_NAME, _canvas);
	
	return true; //return true continues loop
}