#pragma once
#include <algorithm>
#include <cmath>


struct Vector2 {
  float x, y;
  Vector2(float _x = 0, float _y = 0) : x(_x), y(_y) {}
};

struct Vector3 {
  float x, y, z;
  Vector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

  Vector3 operator+(const Vector3 &v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
  }
  Vector3 operator-(const Vector3 &v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
  }
  Vector3 operator*(float f) const { return Vector3(x * f, y * f, z * f); }
  Vector3 operator/(float f) const { return Vector3(x / f, y / f, z / f); }

  float length() const { return std::sqrt(x * x + y * y + z * z); }
  float length_sq() const { return x * x + y * y + z * z; }
  float distance(const Vector3 &v) const { return (*this - v).length(); }
};

struct Matrix4x4 {
  float m[4][4];
};

namespace Math {
inline bool world_to_screen(const Vector3 &pos, Vector2 &screen,
                            const Matrix4x4 &matrix, int width, int height) {
  float w = matrix.m[3][0] * pos.x + matrix.m[3][1] * pos.y +
            matrix.m[3][2] * pos.z + matrix.m[3][3];
  if (w < 0.001f)
    return false;

  float x = matrix.m[0][0] * pos.x + matrix.m[0][1] * pos.y +
            matrix.m[0][2] * pos.z + matrix.m[0][3];
  float y = matrix.m[1][0] * pos.x + matrix.m[1][1] * pos.y +
            matrix.m[1][2] * pos.z + matrix.m[1][3];

  screen.x = (width / 2.0f) + (x / w) * (width / 2.0f);
  screen.y = (height / 2.0f) - (y / w) * (height / 2.0f);
  return true;
}
} // namespace Math
