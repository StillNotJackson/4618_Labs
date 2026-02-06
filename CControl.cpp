#include "stdafx.h"
#include "CControl.h"
#include <string>
#include <sstream>

CControl::CControl()
{

}

CControl::~CControl()
{

}


void CControl::init_com(int comport)
{
	std::string port = "COM" + std::to_string(comport);
	_com.open(port);
}

bool CControl::get_data(int type, int channel, int &result)
{

	std::string rx_cmd = "G " + std::to_string(type) + " " + std::to_string(channel) + "\n";

	_com.write(rx_cmd.c_str(), rx_cmd.length());

	//char rx_buffer[2];
	char rx_buffer = 0;
	std::string rx_str = "";
	double start_time = GetTickCount();

	while ( GetTickCount() - start_time < 1000)/*rx_str[0] != '\n') &&*/
	{
		if (_com.read(&rx_buffer, 1) > 0)
		{
			rx_str += rx_buffer;
			if (rx_buffer == '\n')
			{
				break;
			}
			//Maybe include break if (tx_buffer == \n
		}
	}
	char rx_ack;
	int rx_type, rx_chan, rx_val;

	 
	if (sscanf(rx_str.c_str(), "%c %d %d %d", &rx_ack, &rx_type, &rx_chan, &rx_val) == 4) 
		//https://stackoverflow.com/questions/9537072/properly-using-sscanf basically used this to check that E4618 String is configured properly
	{
		result = rx_val;
		return true;
	}
	return false; //catch all 
}

bool CControl::set_data(int type, int channel, int value)
{
	std::string tx_cmd = "S " + std::to_string(type) + " " + std::to_string(channel) +" " + std::to_string(value) + "\n";

	_com.write(tx_cmd.c_str(), tx_cmd.length());

	char tx_buffer;// [2] ;
	std::string tx_str ="";
	double start_time = GetTickCount();

	//while (GetTickCount() - start_time < 1000 && tx_buffer[0] != '\n')
	while (GetTickCount() - start_time < 1000)
	{
		if (_com.read(&tx_buffer, 1) > 0)
		{
			tx_str += tx_buffer;
			if (tx_buffer == '\n')
			{
				break;
			}

		}
	}
	return (tx_str.size() > 0 && tx_str[0] == 'A');
}

float CControl::get_analog(int channel)
{
	int anal_value = 0;

	if (!get_data(ANALOG, channel, anal_value))
	{
		return -1.0f;
	}
	return (anal_value / 4096.0f * 100.0f);
}

bool CControl::get_button(int channel)
{
	// STATIC BUTTON_STATE MIGHT BE A PROBLEM IF S1 AND S2 ARE CALLED INTERCHANGABLY
	double time_press = 0;

	int button_press;
	static int button_state = 0;//static so it persists between function calls
	
	if (!get_data(DIGITAL, channel, button_press))
	{
		return false;
	}

	if (button_press == 0 && button_state == 1)
	{
		if (GetTickCount() - time_press > 750)//lab says 1 sec but thats too long
		{
			time_press = GetTickCount();
			button_state = button_press;
			return true;
		}
	}
	button_state = button_press;
	return false;
}
