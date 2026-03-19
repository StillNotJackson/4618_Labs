#include "stdafx.h"
#include "CMissile.h"
#include "CGameObject.h"

CMissile::CMissile(cv::Point2f start_pos) 
{
    _radius = 3;
    _position = start_pos;
    _velocity = cv::Point2f(0, -300);
    _lives = 1;
}

void CMissile::draw(cv::Mat& im) 
{
    circle(im, _position, _radius, cv::Scalar(0, 0, 255), -1); // red
}
