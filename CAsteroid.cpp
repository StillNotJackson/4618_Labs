#include "stdafx.h"
#include "CAsteroid.h"

#include <cmath>
#include <cstdlib>

namespace {
    
    const float PI_F = 3.14159;
    inline float get_random() {
        return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    }
}

CAsteroid::CAsteroid(cv::Size board_size) {
    _radius = 15 + static_cast<int>(get_random() * 25.0f);

    // Spawn fully inside bounds
    float x = _radius + get_random() * (board_size.width - 2.0f * _radius);
    float y = _radius + get_random() * (board_size.height - 2.0f * _radius);
    _position = cv::Point2f(x, y);

    //Movement 
    float angle = get_random() * 2.0f * PI_F;
    float speed = 40.0f + get_random() * 140.0f; // px/sec
    _velocity = cv::Point2f(cosf(angle) * speed, sinf(angle) * speed);

    _lives = 1;
}
void CAsteroid::draw(cv::Mat& im) {
    // --- UPDATED: Stony brown/gray fill (BGR: 75, 100, 130) ---
    cv::circle(im, _position, _radius, cv::Scalar(75, 100, 130), -1);

    // --- NEW: Darker brown/gray outline for better visual definition ---
    cv::circle(im, _position, _radius, cv::Scalar(40, 60, 80), 2);
}