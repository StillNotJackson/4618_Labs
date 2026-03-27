#pragma once
#include "CBase4618.h"
#include <vector>
#include <opencv2/core.hpp>
#include "CShip.h"
#include "CMissile.h"
#include "CAsteroid.h"


/**
 * @class CAsteroidGame
 * @brief Main game controller that handles inputs, updates, and rendering.
 *
 * Inherits from CBase4618 to integrate with the existing run-loop framework.
 */
class CAsteroidGame : public CBase4618 {
private:
    cv::Mat _background;
    /// Player ship instance. */
    CShip _ship;

    /// Active asteroids in the current wave. 
    std::vector<CAsteroid> _asteroid_list;

    /// Active missiles fired by the ship.
    std::vector<CMissile> _missile_list;

    /// Player score.
    int _score;

    int _level;

    /// flag for game state
    bool _game_over = false;

    /* Hardware channels (mapped to CControl channels). */
    const int JOYSTICK_X = 11;
    const int JOYSTICK_Y = 4;
    const int BUTTON_FIRE = 32;
    const int BUTTON_RESET = 33;

    /** @brief Last read state for fire button (edge detection if needed). */
    bool _fire_pressed;

    /** @brief Last read state for reset button. */
    bool _reset_pressed;

    /** @brief Maximum allowed asteroids for spawning control. */
    static const int MAX_ASTEROIDS = 6;

    /**
     * @brief Spawn a wave of asteroids.
     * @param count Number of asteroids to spawn.
     */
    void spawn_wave(int count);


    /** @brief Ship maximum movement speed in pixels per second. */
    static constexpr float SHIP_SPEED = 250.0f; // pixels/sec

    /**
     * @brief Wrap an object around screen edges (toroidal space).
     * @param obj Object to wrap.
     *
     * Note: This method updates the object's position using `set_pos`.
     * The comment "boundary reflection" in the original file is preserved.
     */
    void wrap_object(CGameObject& obj);//boundary reflection

    /** @brief Reset game state to initial conditions. */
    void reset_game();

    //Inertia stuff
    cv::Point2f _ship_vel = cv::Point2f(0, 0);
    double _last_tick = 0.0;

public:
    /**
     * @brief Construct the game and initialize hardware communication.
     * @param port Serial port used by CControl.
     */
    CAsteroidGame(int port);

    /** @brief Virtual destructor. */
    ~CAsteroidGame();

    /** @brief Read hardware inputs and update internal input state. */
    void gpio() override;

    /** @brief Advance game simulation (movement, collisions, spawning). */
    void update() override;

    /**
     * @brief Render the current frame and present it to the user.
     * @return true to continue running; false to exit the run-loop.
     */
    bool draw() override;

    bool splash_screen(bool& exit_requested);
};