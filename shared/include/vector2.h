// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef VECTOR2_H
#define VECTOR2_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vector2 {
  float x;
  float y;
} Vector2;

// TODO: (ARC)- Some of these *could* potentially be done in place. In the spirit
//  of allowing vector functions to be called in SIMD/multithreaded environments,
//  the first implementation is all by copy.

// TODO: (ARC) Consider either
//  A) passing in the resultant vectors to be filled
//  B) using arena-backed items for results.

// Add vector a and b. (a + b)
Vector2 Vector2_Add(Vector2 a, Vector2 b);

// Subtract b from a. (a - b)
Vector2 Vector2_Subtract(Vector2 a, Vector2 b);

// Multiply vector by a scalar. (vec * scalar)
Vector2 Vector2_Multiply(Vector2 vec, float scalar);

// Divide a vector by a scalar. Returns Vector2(0.0f, 0.0f) if it can't safely divide.
Vector2 Vector2_Divide(Vector2 vec, float scalar);

// Dot product (a � b).
// Positive == similar direction, negative == opposite direction, 0 == orthoganal.
float Vector2_Dot(Vector2 a, Vector2 b);

// Cross product (a x b). In 2 dimensions this tells us the area of the parallelogram created by the two vectors.
// Sign indicates the orientation of the vectors (clockwise or counterclockwise to each other).
// If a is counterclockwise from b, the resultant sign will be positive.
// If a is clockwise from b, the resultant sign will be negative.
float Vector2_Cross(Vector2 a, Vector2 b);

// Returns the magnitude of a Vector2.
float Vector2_Magnitude(Vector2 vec);

// Returns a normalized version of a Vector2. Returns Vector2(0.0f, 0.0f) if it can't be normalized.
Vector2 Vector2_Normalize(Vector2 vec);

#ifdef __cplusplus
}
#endif

#endif
