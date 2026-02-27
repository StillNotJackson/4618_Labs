#pragma once
#include "CBase4618.h"
#include "cvui.h"
#include <string>

/**
 * @file CSketch.h
 * @brief Etch-A-Sketch style drawing application (ELEX 4618).
 *
 * Implements joystick-controlled cursor motion, reset, and a simple CVUI panel.
 * Input is read from the Tiva-C via CControl and rendered using OpenCV.
 * @author Jack Gillis
 */

 /**
  * @class CSketch
  * @brief Concrete lab application implementing an Etch-A-Sketch.
  *
  * Responsibilities by method:
  * - gpio(): read joystick/buttons from the Tiva-C and store raw inputs
  * - update(): convert joystick input into cursor deltas and clamp to canvas
  * - draw(): draw line segments on the canvas and render the UI overlay
  */
class CSketch : public CBase4618
{
private:
    /// Canvas size in pixels (width x height).
    cv::Size _size;

    /// Current drawing colour in BGR order. Default is red (B=0, G=0, R=255). 
    cv::Scalar _colour = cv::Scalar(0, 0, 255);

    /// Colour selection token/index used to cycle or select preset colours.
    int _colour_token = 0;

    /// Debounce flag for the S2 button. True when S2 was considered pressed on the previous sample.
    bool _s2_state = false; // my stupid fix for debouncing

    /// When true, the next draw() iteration clears the canvas and recentres the cursor. */
    bool _reset = false;

    /// Raw joystick position mapped into canvas coordinates. */
    cv::Point _raw_joystick;

    /// Current cursor position on the canvas. */
    cv::Point _pos_cur;

    /// Previous cursor position (used for drawing line segments). */
    cv::Point _pos_prev;

    /**
     * @brief Create a UI overlay frame to display on top of the canvas.
     * @return A frame containing the canvas plus UI elements (CVUI panel).
     */
    cv::Mat gen_UI();

public:
    /**
     * @brief Construct the sketch application.
     * @param size_canvas Size of the drawing canvas.
     * @param comm_port Windows COM port number used for the Tiva-C connection (e.g., 4 for COM4).
     */
    CSketch(cv::Size size_canvas, int comm_port);
    /** @brief Destructor. */
    ~CSketch();

    ///@copydoc CBase4618::gpio
    void gpio();
    /// @copydoc CBase4618::update 
    void update();
    /// @copydoc CBase4618::draw 
    bool draw(); //bool for exit function

};

