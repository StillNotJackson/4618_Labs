#pragma once
#include "CGameObject.h"

/**
 * @class CAsteroid
 * @brief Randomly spawned moving obstacle.
 *
 * Constructed with the board size so it can spawn fully inside bounds and
 * choose a random direction and speed.
 */
class CAsteroid : public CGameObject {
public:
    /**
     * @brief Create an asteroid positioned randomly within the board.
     * @param board_size Size of the game board used to constrain spawn.
     */
    CAsteroid(cv::Size board_size);
    void draw(cv::Mat& im) override;
};


