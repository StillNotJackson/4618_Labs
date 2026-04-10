#include "CRecyclingSort.h"
#include "CControlPi.h"
#include <iostream>
#include <cmath>

#define DIGITAL 0
#define ANALOG 1
#define SERVO 2

CRecyclingSort::CRecyclingSort()
{
	_comm.init_com(0);
	_video.open(0);
	_video.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	_video.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
	cvui::init("Control");
	cv::namedWindow("Desat. Feed");
	_server.start(4618);
}

CRecyclingSort::~CRecyclingSort()
{
	_video.release();
}

void CRecyclingSort::gpio()
{
	_video >> _frame;

	// Server polling
	std::vector<std::string> cmds;
	_server.get_cmd(cmds);

	for (int i = 0; i < cmds.size(); i++)
	{
		std::string cmd = cmds[i];

		if (cmd == "S 0 1 \n") {
			_system_on = true;
			_auto_mode = true;
		}
		else if (cmd == "S 0 0 \n") {
			_system_on = false;
			_current_bin = "NO OBJECT";
		}
		else if (cmd == "S 1 0 \n") {
			_auto_mode = false;
			_current_bin = "BIN 1";
		}
		else if (cmd == "S 1 1 \n") {
			_auto_mode = false;
			_current_bin = "BIN 2";
		}
		else if (cmd == "G 0 \n") {
			_server.send_string(_system_on ? "System is ON\n" : "System is OFF\n");
		}
		else if (cmd == "G 1 0 \n") {
			_server.send_string(std::to_string(_bin1_count) + "\n");
		}
		else if (cmd == "G 1 1 \n") {
			_server.send_string(std::to_string(_bin2_count) + "\n");
		}
	}

	// Button Debounce
	bool btn_mode = _comm.get_button(22); // CHAN_BTN_MODE
	static bool last_btn_mode = false;

	if (btn_mode && !last_btn_mode) {
		_auto_mode = !_auto_mode; // Toggle state
	}
	last_btn_mode = btn_mode;

	// Manual logic
	if (!_auto_mode) {
		if (_comm.get_button(17)) { // CHAN_BTN_BIN1
			_current_bin = "BIN 1";
		}
		else if (_comm.get_button(27)) { // CHAN_BTN_BIN2
			_current_bin = "BIN 2";
		}
		else {
			_current_bin = "NO OBJECT";
		}
	}

	// LED logic
	_comm.set_data(DIGITAL, 5, _system_on ? 1 : 0); // Mode LED
	_comm.set_data(DIGITAL, 6, (_current_bin == "BIN 1") ? 1 : 0); // Bin 1 LED
	_comm.set_data(DIGITAL, 13, (_current_bin == "BIN 2") ? 1 : 0); // Bin 2 LED

	// Servo logic
	if (_current_bin == "BIN 1") {
		_comm.set_data(SERVO, 19, 1000); // Sorting servo to BIN 1 position
		_comm.set_data(SERVO, 12, 1500); // Open gate servo
	}
	else if (_current_bin == "BIN 2") {
		_comm.set_data(SERVO, 19, 2000); // Sorting servo to BIN 2 position
		_comm.set_data(SERVO, 12, 1500); // Open gate servo
	}
	else {
		_comm.set_data(SERVO, 12, 1000); // Close gate servo (block flow)
	}
}

void CRecyclingSort::update()
{
	if (_frame.empty()) return;

	if (!_system_on && _auto_mode) {
		_current_bin = "NO OBJECT";
		return;
	}

	// Convert frames for processing
	cv::cvtColor(_frame, _gray_frame, cv::COLOR_BGR2GRAY);
	cv::cvtColor(_frame, _hsv_frame, cv::COLOR_BGR2HSV);

	// Color Definitions for Auto-Detection
	struct ColorDef { std::string name; cv::Scalar low; cv::Scalar high; };

	std::vector<ColorDef> color_list = {
		{"GREEN", cv::Scalar(30, 80, 80), cv::Scalar(85, 255, 255)},
		{"BLUE", cv::Scalar(90, 80, 80), cv::Scalar(135, 255, 255)},
		{"ORANGE", cv::Scalar(0, 80, 80), cv::Scalar(25, 255, 255)},
		{"PINK", cv::Scalar(140, 80, 80), cv::Scalar(179, 255, 255)}
	};

	int frame_center_x = _frame.cols / 2;
	int frame_center_y = _frame.rows / 2;
	const double AREA_THRESH = 1500;
	const double MAX_RADIUS = 220;

	double min_distance = 1e9;
	std::string detected_color = "NONE";
	cv::Point target_centroid(0, 0);
	std::vector<cv::Point> best_contour;

	// 1. Process Image to find the largest object of target color [cite: 19]
	for (auto& c : color_list) {
		cv::Mat temp_mask;
		cv::inRange(_hsv_frame, c.low, c.high, temp_mask);
		cv::erode(temp_mask, temp_mask, cv::Mat(), cv::Point(-1, -1), 2);
		cv::dilate(temp_mask, temp_mask, cv::Mat(), cv::Point(-1, -1), 2);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(temp_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		for (auto& cnt : contours) {
			if (cv::contourArea(cnt) > AREA_THRESH) {
				cv::Moments M = cv::moments(cnt);

				if (M.m00 > 0) {
					int cx = int(M.m10 / M.m00);
					int cy = int(M.m01 / M.m00);
					double distance = std::sqrt(std::pow(cx - frame_center_x, 2) + std::pow(cy - frame_center_y, 2));

					if (distance > MAX_RADIUS) continue;

					if (distance < min_distance) {
						min_distance = distance;
						detected_color = c.name;
						target_centroid = cv::Point(cx, cy);
						best_contour = cnt;
					}
				}
			}
		}
	}

	_target_distance = min_distance;

	// 2. Trackbar mask for BIN 1 segmentation [cite: 42, 43]
	cv::inRange(_hsv_frame, cv::Scalar(_hue_low, 100, 100), cv::Scalar(_hue_high, 255, 255), _mask);
	cv::erode(_mask, _mask, cv::Mat(), cv::Point(-1, -1), 1);
	cv::dilate(_mask, _mask, cv::Mat(), cv::Point(-1, -1), 1);

	// 3. Automated Sorting Logic [cite: 11, 20]
	// Only update _current_bin automatically if system is in AUTO mode [cite: 49, 50]
	if (_auto_mode) {
		std::string auto_bin = "NO OBJECT"; // [cite:44]

		if (detected_color != "NONE") {
			_target_box = cv::boundingRect(best_contour);

			// If the centroid of the detected color falls within the user-defined Hue range, it's BIN 1 [cite: 43]
			if (_mask.at<uchar>(target_centroid.y, target_centroid.x) > 0) {
				auto_bin = "BIN 1";
			}
			else {
				auto_bin = "BIN 2";
			}
		}

		_current_bin = auto_bin;
	}

	_current_color = detected_color;

	// 4. Object Counting Logic
	static std::string last_bin = "NO OBJECT";
	if (_current_bin != last_bin) {
		if (_current_bin == "BIN 1") {
			_bin1_count++;
		}
		else if (_current_bin == "BIN 2") {
			_bin2_count++;
		}

		// Print status to console
		if (_current_bin == "NO OBJECT") {
			std::cout << "System Status: IDLE" << std::endl;
		}
		else {
			std::cout << "Action: Sorting " << _current_color << " to " << _current_bin << std::endl;
		}

		last_bin = _current_bin;
	}
}

bool CRecyclingSort::draw()
{
	if (_frame.empty()) return true;

	// UI Setup
	cv::Mat ui_frame = cv::Mat(250, 400, CV_8UC3, cv::Scalar(49, 49, 49));

	cvui::text(ui_frame, 20, 20, "Hue Low Thres.");
	cvui::trackbar(ui_frame, 20, 40, 350, &_hue_low, 0, 179);

	cvui::text(ui_frame, 20, 100, "Hue High Thres.");
	cvui::trackbar(ui_frame, 20, 120, 350, &_hue_high, 0, 179);

	cvui::printf(ui_frame, 20, 190, "Range: %d to %d", _hue_low, _hue_high);

	cvui::checkbox(ui_frame, 20, 220, "Auto Mode", &_auto_mode);

	if (!_auto_mode) {
		if (cvui::button(ui_frame, 20, 260, "Sort BIN 1")) {
			_current_bin = "BIN 1";
		}

		if (cvui::button(ui_frame, 150, 260, "Sort BIN 2")) {
			_current_bin = "BIN 2";
		}
	}

	cvui::update("Control");
	cvui::imshow("Control", ui_frame);

	// Display video frame
	cv::Mat result;
	cv::cvtColor(_gray_frame, result, cv::COLOR_GRAY2BGR);

	// Bounding box
	if (_current_bin != "NO OBJECT") {
		cv::rectangle(result, _target_box, cv::Scalar(0, 255, 0), 3);
	}

	// Center point
	cv::drawMarker(result, cv::Point(result.cols / 2, result.rows / 2), cv::Scalar(255, 255, 255), cv::MARKER_CROSS, 20, 2);

	// Put the status text on the frame
	std::string display_text = (_current_bin == "NO OBJECT") ? "NO OBJECT PRESENT" : (_current_color + " (" + _current_bin + ")");
	cv::Scalar text_color = (_current_bin == "NO OBJECT") ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 0);

	cv::putText(result, display_text, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 1.0, text_color, 3);

	_server.set_txim(result); // Pass the final drawn frame to the socket

	// Windows display
	cv::imshow("Recycling Sorter", result);
	cv::imshow("Box Cam", _frame); // Shows what the camera sees for troubleshooting

	return true;
}