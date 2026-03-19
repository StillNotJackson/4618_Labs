#include "stdafx.h"
#include "CRecyclingSort.h"


CRecyclingSort::CRecyclingSort()
{
	// TODO: Init Camera object and cvui

}

CRecyclingSort::~CRecyclingSort()
{
}

void CRecyclingSort::gpio()
{//TODO: Read camera frame and store in _frame
	// Activate servos depending on item # flag

}

void CRecyclingSort::update()
{
	//TODO: 1. Convert m_frame to HSV color space (m_hsv_frame)
    // 2. Use cv::inRange with m_hue_low and m_hue_high to create m_mask
    // 3. Apply cv::erode and cv::dilate on m_mask to remove noise
    // 4. Use cv::findContours on the mask
    // 5. Calculate contour areas to find the largest one
    
    // Logic implementation:
    // - If largest contour area > threshold: BIN 1
    // - Else if any other object detected (using a grayscale threshold, for example) > threshold: BIN 2
    // - Else: NO object (DO NOTHING)

    if (_frame.empty()) //catch for empty frame
    {
        return;
	}
	cv::cvtColor(_frame, _hsv_frame, cv::COLOR_BGR2HSV); //convert to HSV color space

	cv::inRange(_hsv_frame, cv::Scalar(_hue_low, 100, 100), cv::Scalar(_hue_high, 255, 255), _mask); //create mask based on hue range
	cv::erode(_mask, _mask, cv::Mat(), cv::Point(-1, -1), 2); //erode to remove noise
	cv::dilate(_mask, _mask, cv::Mat(), cv::Point(-1, -1), 2); //dilate to restore object size

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); //find contours in the mask


	// Find the largest contour area
	double max_area = 0;
	int max_contour_index = -1;
    for (double i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area > max_area)
        {
            max_area = area;
            max_contour_index = i;
        }
	}

}

bool CRecyclingSort::draw()
{
    // 1. Create a display frame
    // 2. Add cvui::trackbar for m_hue_low
    // 3. Add cvui::trackbar for m_hue_high
    // 4. Show the original frame and the masked result
    // 5. Draw the contour outline of the detected object
    // 6. Print system status (BIN 1, BIN 2, or NO OBJECT) using cvui::printf

	cv::imshow("Recycling Sorter", _frame);

	// return false if user presses 'q' or 'Q' or closes the window

	return true;
}

