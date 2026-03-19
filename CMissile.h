#pragma once
#include "CGameObject.h" // Ensure CGameObject is a complete type at inheritance point.

/**
 * @class CMissile
 * @brief Short-lived projectile fired by the ship.
 *
 * Missiles move quickly upward and have a small radius.
 */
class CMissile : public CGameObject
{
public:
    /**
     * @brief Construct a missile starting at `start_pos`.
     * @param start_pos Initial position of the missile (typically the ship's position).
     */
    CMissile(cv::Point2f start_pos);

    /**
     * @brief Draw the missile onto the provided image.
     * @param im Image to draw onto.
     */
    void draw(cv::Mat& im) override;
};

