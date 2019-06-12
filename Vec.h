#ifndef VEC_H
#define VEC_H

#include <cmath>

struct Vec
{
    Vec() : x(0.0), y(0.0), z(0.0) {}
    Vec(float f[3]) : x(f[0]), y(f[1]), z(f[2]) {}
    Vec(float f) : x(f), y(f), z(f) {}
    Vec(float a, float b, float c) : x(a), y(b), z(c) {}

    Vec operator+(float f) const { return Vec(x+f, y+f, z+f); }
    Vec operator-(float f) const { return Vec(x-f, y-f, z-f); }
    Vec operator*(float f) const { return Vec(x*f, y*f, z*f); }
    Vec operator/(float f) const { return Vec(x/f, y/f, z/f); }

    Vec operator+(const Vec& v) const { return Vec(x+v.x, y+v.y, z+v.z); }
    Vec operator-(const Vec& v) const { return Vec(x-v.x, y-v.y, z-v.z); }
    Vec operator*(const Vec& v) const { return Vec(x*v.x, y*v.y, z*v.z); }
    Vec operator/(const Vec& v) const { return Vec(x/v.x, y/v.y, z/v.z); }

    float dot(const Vec& v) const { return x*v.x + y*v.y + z*v.z; }

    Vec cross(const Vec& v) const 
    { 
        return Vec(
            y*v.z - z*v.y,
            z*v.x - x*v.z,
            x*v.y - y*v.x
        );
    }

    float mag() const
    {
        return std::sqrt(x*x + y*y + z*z);
    }

    Vec& normalize()
    {
        float m = mag();
        x /= m;
        y /= m;
        z /= m;
        return *this;
    }

    float x;
    float y;
    float z;
};

inline void createLocalCoord(const Vec &n, Vec &u, Vec &v, Vec &w) {
    w = n;

    if (std::abs(n.x) > std::abs(n.y))
        v = Vec(-n.z, 0, n.x)/sqrt(n.z*n.z + n.x*n.x);
    else
        v = Vec(0, n.z, -n.y)/sqrt(n.z*n.z + n.y*n.y);

    u = v.cross(w);
}

struct Quat
{
    Quat() {}
    Quat(float f[4]) : x(f[0]), y(f[1]), z(f[2]), r(f[3]) {}
    Quat(const Vec& c, float a) : x(c.x), y(c.y), z(c.z), r(a) {}

    float x;
    float y;
    float z;
    float r;
};

#endif
