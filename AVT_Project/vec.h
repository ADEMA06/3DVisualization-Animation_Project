#ifndef __VEC_H__
#define __VEC_H__
#include <cmath>

struct vec3 {
    float x;
    float y;
    float z;

    vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    vec3 normalize() {
        vec3 normalized = { x, y, z };
        float magnitude = std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
        if (magnitude != 0)
        {
            normalized.x /= magnitude;
            normalized.y /= magnitude;
            normalized.z /= magnitude;
        }

        return normalized;

    }

    friend vec3 operator+(const vec3& lvalue, const vec3& rvalue) {
        vec3 res;
        res.x = lvalue.x + rvalue.x;
        res.y = lvalue.y + rvalue.y;
        res.z = lvalue.z + rvalue.z;

        return res;
    }

    friend vec3 operator-(const vec3& lvalue, const vec3& rvalue) {
        vec3 res;
        res.x = lvalue.x - rvalue.x;
        res.y = lvalue.y - rvalue.y;
        res.z = lvalue.z - rvalue.z;

        return res;
    }

    friend vec3 operator*(const vec3& lvalue, const float& rvalue) {
        vec3 res;
        res.x = lvalue.x * rvalue;
        res.y = lvalue.y * rvalue;
        res.z = lvalue.z * rvalue;

        return res;
    }


};

struct vec4 {
    float x;
    float y;
    float z;
    float w;

    vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    friend vec4 operator+(const vec4& lvalue, const vec4& rvalue) {
        vec4 res;
        res.x = lvalue.x + rvalue.x;
        res.y = lvalue.y + rvalue.y;
        res.z = lvalue.z + rvalue.z;
        res.w = lvalue.w + rvalue.w;

        return res;
    }

    friend vec4 operator-(const vec4& lvalue, const vec4& rvalue) {
        vec4 res;
        res.x = lvalue.x - rvalue.x;
        res.y = lvalue.y - rvalue.y;
        res.z = lvalue.z - rvalue.z;
        res.w = lvalue.w - rvalue.w;

        return res;
    }
};


#endif