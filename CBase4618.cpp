#include "stdafx.h"
#include "CBase4618.h"
#include "cvui.h"
#include "Lab3.h"


CBase4618::CBase4618()
{

}


CBase4618::~CBase4618()
{

}

void CBase4618::run()
{
	while (true)    //while(true) is bad, but lab3::user_exit sould cya
	{
		gpio();
		update();

		if (!draw())//draw returns false on user quit
		{
			break;
		}

		// key = cv::waitKey(10);
		if (user_exit())//might have a linker problem
		{
			break;
		}
	}
}