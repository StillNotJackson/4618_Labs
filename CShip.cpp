#include "stdafx.h"
#include "CShip.h"


CShip::CShip() 
{
    _radius = 20;
    _lives = 10;
    _position = cv::Point2f(400, 300);
    _velocity = cv::Point2f(0, 0);
    _angle = 0.0f;
}
void CShip::draw(cv::Mat& im)
{
    // triangle size
    float tipLen = _radius * 1.6f;   // forward tip distance
    float backLen = _radius * 1.0f;  // how far back the base is
    float halfW = _radius * 0.9f;    // half width of base

    // forward and right vectors from angle
    cv::Point2f f(cosf(_angle), sinf(_angle));
    cv::Point2f r(-sinf(_angle), cosf(_angle));

    // 3 vertices (float)
    cv::Point2f tip = _position + f * tipLen;
    cv::Point2f left = _position - f * backLen - r * halfW;
    cv::Point2f right = _position - f * backLen + r * halfW;

    // convert to integer points and use a vector to match cv::polylines overload
    std::vector<cv::Point> pts = {
        cv::Point(static_cast<int>(tip.x),  static_cast<int>(tip.y)),
        cv::Point(static_cast<int>(left.x), static_cast<int>(left.y)),
        cv::Point(static_cast<int>(right.x),static_cast<int>(right.y))
    };

    // --- UPDATED: Filled triangle in Silver Starship Gray (BGR: 192, 192, 192) ---
    cv::fillConvexPoly(im, pts.data(), static_cast<int>(pts.size()), cv::Scalar(192, 192, 192));

    // outline using the vector overload to avoid overload resolution errors
    // (Kept black for sharp contrast against the starfield)
    cv::polylines(im, pts, true, cv::Scalar(0, 0, 0), 2);
}