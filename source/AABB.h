#pragma once
#include "Vector2.h"

class AABB
{
protected:
    Vector2 _topLeft;
    Vector2 _size;

public:
    AABB(Vector2 topLeft, Vector2 size)
        : _topLeft(topLeft), _size(size) {
    }

    Vector2 GetTopLeft() const { return _topLeft; }
    Vector2 GetSize() const { return _size; }
    void SetTopLeft(Vector2 newTopLeft) { _topLeft = newTopLeft; }
    void SetSize(Vector2 newSize) { _size = newSize; }

    bool CheckOverlappingPoint(Vector2 point)
    {
        bool insideX = point.x >= _topLeft.x && point.x <= _topLeft.x + _size.x;
        bool insideY = point.y >= _topLeft.y && point.y <= _topLeft.y + _size.y;

        return insideX && insideY;
    }

    bool CheckOverlappingAABB(const AABB* other)
    {
        bool intersectionX = false;
        intersectionX |= _topLeft.x >= other->_topLeft.x && _topLeft.x <= other->_topLeft.x + other->_size.x;
        intersectionX |= other->_topLeft.x >= _topLeft.x && other->_topLeft.x <= _topLeft.x + _size.x;

        bool intersectionY = false;
        intersectionY |= _topLeft.y >= other->_topLeft.y && _topLeft.y <= other->_topLeft.y + other->_size.y;
        intersectionY |= other->_topLeft.y >= _topLeft.y && other->_topLeft.y <= _topLeft.y + _size.y;

        return intersectionX && intersectionY;
    }
};