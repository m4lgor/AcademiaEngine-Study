#pragma once
#include "../../external/olc/olcPixelGameEngine.h"

// Our names for the engine's 2D vectors. The rest of the code only uses these names.
//
// Vector2f  floats. World positions and sizes. The default choice.
// Vector2d  doubles. Same as Vector2f with more precision, for calculations that add up small errors.
// Vector2i  signed ints. Pixel positions on the screen.
// Vector2u  unsigned ints. Sizes and counts that can never be negative.
//
// Every vector has x and y, the usual operators (+ - * /, +=, ==) and these functions:
//   mag() length, mag2() length squared, norm() unit vector, perp() rotated 90 degrees,
//   dot(v), cross(v) (a single number in 2D), reflect(normal), lerp(v, t),
//   min(v), max(v) and clamp(lowest, highest), which work on x and y separately.
using Vector2f = olc::vf2d;
using Vector2d = olc::vd2d;
using Vector2i = olc::vi2d;
using Vector2u = olc::vu2d;
