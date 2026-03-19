#include "stdafx.h"
#include "CGameObject.h"
#include "cvui.h"




CGameObject::CGameObject() {
    _last_tick = (double)cv::getTickCount();
    _position = cv::Point2f(0, 0);
    _velocity = cv::Point2f(0, 0);
    _radius = 1;
    _lives = 1;
}

void CGameObject::move() {
    double current_tick = (double)cv::getTickCount();
    double dt = (current_tick - _last_tick) / cv::getTickFrequency();
    _last_tick = current_tick;

    _position.x += (float)(dt * _velocity.x);
    _position.y += (float)(dt * _velocity.y);
}

bool CGameObject::collide(CGameObject& obj) {
    double dist = norm(_position - obj.get_pos());
    return dist < (_radius + obj._radius);
}

bool CGameObject::collide_wall(cv::Size board) {
    if (_position.x - _radius < 0 || _position.x + _radius > board.width ||
        _position.y - _radius < 0 || _position.y + _radius > board.height) {
        return true;
    }
    return false;
}

void CGameObject::hit() {
    _lives--;
}

void CGameObject::draw(cv::Mat& im) {
    circle(im, _position, _radius, cv::Scalar(255, 255, 255), -1);
}

