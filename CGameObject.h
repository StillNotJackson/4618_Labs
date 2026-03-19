#pragma once
#include <opencv2/opencv.hpp>



/**
 * @class CGameObject
 * @brief Base class for all moving objects in the game.
 *
 * Provides basic physics (position, velocity, radius) and simple collision /
 * lifetime management used by derived game objects.
 */
class CGameObject 
{
protected:
    /** @brief Current position in pixels (screen coordinates). */
    cv::Point2f _position;

    /** @brief Current velocity in pixels per second. */
    cv::Point2f _velocity;

    /** @brief Collision radius in pixels. */
    int _radius;

    /** @brief Remaining lives / hit points for the object. */
    int _lives;

    /** @brief Last tick used for frame-time based movement. */
    double _last_tick;

public:
    /** @brief Construct a default game object at origin with zero velocity. */
    CGameObject();

    /** @brief Advance the object's position by its velocity using elapsed time. */
    void move();

    /**
     * @brief Test collision against another game object using circle intersection.
     * @param obj Other game object to test against.
     * @return true if the two objects overlap (distance < sum of radii).
     */
    bool collide(CGameObject& obj);

    /**
     * @brief Test whether the object intersects the board boundaries.
     * @param board Size of the board (width, height).
     * @return true if any part of the object lies outside the board.
     */
    bool collide_wall(cv::Size board);

    /** @brief Apply a hit to the object (decrement lives). */
    void hit();

    /** @brief Get remaining lives/hit points. */
    int get_lives() { return _lives; }

    /**
     * @brief Set remaining lives/hit points.
     * @param lives New lives value.
     */
    void set_lives(int lives) { _lives = lives; }

    /**
     * @brief Set object position.
     * @param pos New position in screen coordinates.
     */
    void set_pos(cv::Point2f pos) { _position = pos; }

    /**
     * @brief Get object position.
     * @return Current position as cv::Point2f.
     */
    cv::Point2f get_pos() { return _position; }

    /**
     * @brief Set object velocity.
     * @param velocity New velocity in pixels/sec.
     *
     * Public setter is provided so controllers can change velocity without
     * accessing protected members directly.
     */
    void set_velocity(cv::Point2f velocity) { _velocity = velocity; }

    /**
     * @brief Draw the object onto the provided image.
     * @param im Destination image (BGR).
     *
     * Default implementation draws a filled white circle centered at _position.
     * Derived classes should override to provide custom appearance.
     */
    virtual void draw(cv::Mat& im);

    cv::Point2f get_velocity() const { return _velocity; }
};

