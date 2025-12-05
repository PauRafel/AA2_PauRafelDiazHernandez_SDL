#pragma once
#include <cmath>

class Vector2
{
public:
    float x;
    float y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}

    void Normalize()
    {
        float length = sqrtf(x * x + y * y);
        if (length > 0.0f)
        {
            x /= length;
            y /= length;
        }
    }

    float Magnitude() const
    {
        return sqrtf(x * x + y * y);
    }

    Vector2 operator+(const Vector2& other) const
    {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const
    {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(const float& scalar) const
    {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator*(const Vector2& other) const
    {
        return Vector2(x * other.x, y * other.y);
    }

    Vector2 operator/(const float& scalar) const
    {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2& operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(const float& scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }
};