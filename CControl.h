#pragma once
#include "Serial.h"

/**
*
* @brief CControl class sets up serial communicate with embedded system on tiva-c board
* This class uses the ELEX 4618 Communication Protocol, a bootleg of the NMEA 0813 design
* It facilitates the digital, analog, and servo operations on the TIVA board
*
* @author Jack Gillis
**/

enum type { DIGITAL = 0, ANALOG, SERVO };

class CControl
{
private:

	///@brief Serial port communication object. Will transmit and recieve data over COMport
	Serial _com;

	/*
	std::string _port;
	bool _connected = false;
	int connection_retry = 0;
	*/

	bool check_connection();
	bool board_response();

public:
	
	///@brief Constuctor for CControl object
	CControl();
	                                                                                                                                                                                                                                                                                                                                                                                                   
	
	///@brief Destructor, will terminate the serial port
	~CControl();

	/** @brief Initilizes the serial port to the comm port by passing the parameter to the method.
	* 
	* @param comport The comport through which the TIVA-C board is connected to (i.e. 4 for COM4).
	* @return nothing to return.
	*/
	void init_com(int comport);
	
	/** @brief Getter for the data coming from the _com object
	* @param type Dictates the command type the user is expecting from the board. 0 = Digitial, 1 = Analog, and 2 = Servo.
	* @param channel Channel number
	* @param result Recieved data from the board
	* @return True upon successful reception of data, Flase on timeout or error.
	*/
	bool get_data(int type, int channel, int &result);

	/** @brief Setter for the data coming from the _com object
	* @param type Dictates the command type the user is sending to the board. 0 = Digital, 1 = Analog (will return false since the TIVA-C can't take analog input), 2 = servo
	* @param channel Channel number for board
	* @param val Value to write
	* @return Returns True upon success, False for timeout or error.
	*/
	bool set_data(int type, int channel, int val);

	/**
	* @brief Reads analog input into board as a percentage of 12-bit ADC (4096)
	* @param channel Analog channel number.
	* @return Analog input from joystick as percentage. 50% is neutral position.
	*/
	float get_analog(int channel);


	/**
	* @brief Reads digitial input and debounces it
	* @param channel Digital channel of button
	* @return True on sucessful read of button, false on fail
	*/
	bool get_button(int channel);
};

