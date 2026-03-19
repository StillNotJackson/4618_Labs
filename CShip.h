#pragma once
#include "CGameObject.h" 


/**
 * @class CShip
 * @brief Player-controlled ship object.
 *
 * Initializes with a larger radius and more lives. Draws as a green circle.
 */
class CShip : public CGameObject {
public:
    CShip();
    void draw(cv::Mat& im) override;
    void set_angle(float rad) { _angle = rad; }
    float get_angle() const { return _angle; }

private:
    float _angle = 0.0f;   // radians, 0 = pointing right
};


