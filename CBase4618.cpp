#include "stdafx.h"
#include "CBase4618.h"
#include "cvui.h"
#include "Lab3.h"

#define CHAN_LED_RED 39
#define CHAN_LED_GRN 38
#define CHAN_LED_BLU 37

CBase4618::CBase4618()
{

}


CBase4618::~CBase4618()
{

}

void CBase4618::run()
{
	bool quit_flag = false;
	while (true)    //while(true) is bad, but lab3::user_exit sould cya
	{
		gpio();
		update();

		if (!draw())//draw returns false on user quit
		{
			break;
		}

		int key = cv::waitKey(1);
		if (key == 'q' || key == 'Q')
		{
			break;
		}


		/*
		if (user_exit())//might have a linker problem
		{
			break;
		}
		*/
	}
	cv::destroyAllWindows();
	_comm.set_data(DIGITAL, CHAN_LED_RED, 0);
	_comm.set_data(DIGITAL, CHAN_LED_GRN, 0);
	_comm.set_data(DIGITAL, CHAN_LED_BLU, 0);
}
