#pragma once

#include <opencv2/core.hpp>
#include "CControl.h"


/**
 * @file CBase4618.h
 * @brief Abstract base class for ELEX 4618 OpenCV/CVUI labs.
 *
 * Provides a common run-loop that repeatedly calls gpio(), update(), and draw()
 * until the user quits (Exit button or 'q' key). Derived labs implement the
 * three pure-virtual methods.
 * @author Jack Gillis
 */

 /**
  * @class CBase4618
  * @brief Common framework for ELEX 4618 lab applications.
  *
  * Holds the serial communication object (CControl) and the drawing canvas.
  * Derived classes are responsible for reading inputs in gpio(), advancing game /
  * sketch state in update(), and rendering in draw().
  */
class CBase4618
{
protected:
	CControl _comm;
	cv::Mat _canvas;

	bool _user_exit = false;

public:
	/** @brief Construct the base object. */
	CBase4618();
	/** @brief Virtual destructor for safe polymorphic deletion. */
	~CBase4618();

	/** @brief Handles all communication between microcontroller and program. 
	* Stores inputs into member variables for usage in update().*/
	virtual void gpio() = 0;

	/**@brief Update application state using the most recent inputs.*/
	virtual void update() = 0;

	
	/** @brief Render the current frame and handle UI interactions.
	 * @return true to continue running; false to exit the run-loop.
	*/
	virtual bool draw() = 0;

	/**@brief Main application loop.
	*
	* Calls gpio() and update() each iteration, then calls draw().
	* Exits when draw() returns false or when the user presses 'q' / 'Q'.
	*/
	void run();

};