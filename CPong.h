#pragma once
#include "CBase4618.h"
#include <opencv2/opencv.hpp>

/**
 * @file CPong.h
 * @brief Pong game class header.
 *
 * Provides the CPong game implementation which derives from CBase4618.
 * The class contains game state (ball, paddles, scores), timing and
 * methods to run the game loop, handle GPIO polling and render frames.
 * @author Jack Gillis
 */
class CPong : public CBase4618
{
private:
    /// Main drawing canvas for the game.
    cv::Mat _canvas;

    /// Canvas size (width x height).
	cv::Size _size = cv::Size(1000, 800);

    /// Mutex used to protect shared state between threads.
    std::mutex _mutex;

    /// Flag indicating whether the game loop / threads should keep running.
    bool _is_running;

    /// When true the game will be reset on the next frame.
    bool _reset = false;

    // Ball state
    /// Current ball position (floating point for smooth movement).
    cv::Point2f _ball_pos;
    /// Current ball direction vector (normalized-ish).
    cv::Point2f _ball_dir;
    /// Ball radius in pixels.
    int _ball_radius;
    /// Ball speed in pixels per second (used with delta time).
    int _ball_speed;

    // Paddle & score
    /// Vertical position (center y) of the player's paddle (right side).
    int _paddle_player_y;
    /// Vertical position (center y) of the PC's paddle (left side).
    int _paddle_pc_y;
    /// Player score.
    int _score_player;
    /// Computer score.
    int _score_pc;

    // Timing
    /// Last tick value used to compute delta time.
    double _last_tick;
    /// Measured frames per second (derived from delta time).
    double _fps;

public:
    /**
     * @brief Construct a Pong game instance.
     * @param comm_port COM port number passed to the underlying CControl in CBase4618.
     *
     * The constructor initializes the canvas, communication and default game state.
     */
    CPong(int comm_port);

    /**
     * @brief Destructor.
     *
     * Ensures any resources owned by CPong are released.
     */
    ~CPong();

    /**
     * @brief Start the main game loop.
     *
     * Overrides CBase4618::run(). This method will create any internal threads
     * required (GPIO polling) and enter the update/draw loop until the user exits.
     */
    void run() override;

    /**
     * @brief Legacy / alternate update thread entry point.
     *
     * Intended to be launched as a detached thread to poll GPIO at a slower rate.
     * The implementation should lock _mutex before touching shared state.
     */
    void update_thread();

    /**
     * @brief Faster GPIO polling thread entry point.
     *
     * Typically used to spawn a dedicated thread that calls gpio() frequently.
     * The implementation should lock _mutex when accessing shared game state.
     */
    void update_gpio();

    /**
     * @brief (Reserved) Draw thread entry point.
     *
     * Present in the header for completeness; current implementation draws on
     * the main thread. If you implement a separate draw thread, ensure proper
     * synchronization with _mutex.
     */
    void draw_thread();

    /**
     * @brief Read inputs from the hardware (joystick, buttons).
     *
     * gpio() should be non-blocking or be executed in a dedicated thread.
     * It is responsible for updating paddle positions and button-driven flags.
     */
    void gpio();

    /**
     * @brief Advance game physics/logic by one time step.
     *
     * update() should compute delta time (using _last_tick) and move the ball,
     * detect collisions, update scores and constrain paddle positions.
     */
    void update();

    /**
     * @brief Render the current frame into _canvas and display it.
     * @return true to continue running; false to exit the main loop.
     *
     * draw() is responsible for composing the game scene and UI and calling
     * cv::imshow()/cvui::update() as appropriate.
     */
    bool draw();

    /**
     * @brief Reset game state to initial values.
     *
     * Resets ball position/direction, scores and paddle positions.
     */
    void reset();
	
};

