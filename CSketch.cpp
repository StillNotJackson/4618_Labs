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
	_comm.set_data(DIGITAL, CHAN_LED_RED, 1);

	_canvas = cv::Mat::zeros(_size, CV_8UC3);//8 unsigned bits with 3 channels
	cvui::init("Etch-A-Sketch");
	

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

	bool S2_cur = _comm.get_button(CHAN_S2);
	if (S2_cur && !_s2_state)
	{
		_colour_token++;
		//if (_colour_token > 2) _colour_token = 0;
		switch (_colour_token % 3)
		{
		case 0://red
		{
			_colour = cv::Scalar(0, 0, 255);
			_comm.set_data(DIGITAL, CHAN_LED_BLU, 0);
			_comm.set_data(DIGITAL, CHAN_LED_RED, 1);
			break;
		}
		case 1://green
		{
			_colour = cv::Scalar(0, 255, 0);
			_comm.set_data(DIGITAL, CHAN_LED_RED, 0);
			_comm.set_data(DIGITAL, CHAN_LED_GRN, 1);
			break;
		}
		case 2://blue
		{
			_colour = cv::Scalar(255, 0, 0);
			_comm.set_data(DIGITAL, CHAN_LED_GRN, 0);
			_comm.set_data(DIGITAL, CHAN_LED_BLU, 1);
			//_colour_token = _colour_token - 3;
			break;
		}
		}
	}
	_s2_state = S2_cur;
}
void CSketch::update()
{
	_pos_prev = _pos_cur;

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

	if (_pos_cur.y < 0)
		_pos_cur.y = 0;
	if (_pos_cur.y > _size.height - 1)
		_pos_cur.y = _size.height - 1;


}
bool CSketch::draw()
{
	// Praise this man https://fernandobevilacqua.com/cvui/components/button/
	cvui::context(WINDOW_NAME);

	cv::Mat frame = gen_UI();

	if (cvui::button(frame, 100, 65, 70, 25, "Exit"))
	{
		return false;//kills loop
	}
	if (cvui::button(frame, 20, 65, 70, 25, "Reset"))
	{
		_reset = true;
	}

	if (_reset)
	{
		_canvas = cv::Mat::zeros(_size, CV_8UC3);
		_pos_cur = cv::Point(_size.width / 2, _size.height / 2);
		_pos_prev = _pos_cur;

		frame = gen_UI();

		_reset = false;
	}
	//cv::line(_canvas, _pos_prev, _pos_cur, _colour, 2);//Might need a funciton for _colour
	cv::line(_canvas, _pos_prev, _pos_cur, _colour, 2);
	cvui::update(WINDOW_NAME);
	//cv::imshow(WINDOW_NAME, _canvas);
	cv::imshow(WINDOW_NAME, frame);

	return true; //return true continues loop
}

cv::Mat CSketch::gen_UI()
{
	const int UI_lft = 10, UI_rgt = 170, UI_top = 10, UI_bot = 90;
	std::string colour_name = "Red";


	cv::Mat frame = _canvas.clone();
	cv::rectangle(frame, cv::Rect(UI_lft, UI_top, UI_rgt, UI_bot), cv::Scalar(60, 60, 60), cv::FILLED);
	cvui::text(frame, UI_lft + 10, UI_top + 12, WINDOW_NAME);

	switch (_colour_token % 3)
	{
	case 0:
	{
		colour_name = "Red";
		break;
	}
	case 1:
	{
		colour_name = "Green";
		break;
	}
	case 2:
	{
		colour_name = "Blue";
		break;
	}
	}
	cvui::text(frame, UI_lft + 10, UI_top + 32, "Colour: " + colour_name);

	return frame;
}