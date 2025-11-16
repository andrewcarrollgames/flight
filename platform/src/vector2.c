// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "vector2.h"
#include <math.h>

#define EPSILON 1e-6f // Tolerance

Vector2 Vector2_Add(const Vector2 a, const Vector2 b) {
  return (Vector2){.x = a.x + b.x, .y = a.y + b.y};
}

Vector2 Vector2_Subtract(const Vector2 a, const Vector2 b) {
  return (Vector2){.x = a.x - b.x, .y = a.y - b.y};
}

Vector2 Vector2_Multiply(const Vector2 vec, const float scalar) {
  return (Vector2){vec.x * scalar, vec.y * scalar};
}

Vector2 Vector2_Divide(const Vector2 vec, const float scalar) {
  if (scalar >= EPSILON) {
    const float invScalar = 1.0f / scalar;
    return (Vector2){vec.x * invScalar, vec.y * invScalar};
  }
  return (Vector2){.x = 0.0f, .y = 0.0f};
}

float Vector2_Dot(const Vector2 a, const Vector2 b) {
  return (a.x * b.x) + (a.y * b.y);
}

float Vector2_Cross(const Vector2 a, const Vector2 b) {
  return (a.x * b.y) - (a.y * b.x);
}

float Vector2_Magnitude(const Vector2 vec) {
  return sqrtf((vec.x * vec.x) + (vec.y * vec.y));
}

Vector2 Vector2_Normalize(const Vector2 vec) {
  const float mag = Vector2_Magnitude(vec);
  return (fabsf(mag) >= EPSILON) ? (Vector2){.x = vec.x / mag, .y = vec.y / mag} : (Vector2){.x = 0.0f, .y = 0.0f};
}
