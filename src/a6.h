/*
    CS 89/189 Computational Aspects of Digital Photography C++ basecode.

    Adapted from MIT's 6.815/6.865 basecode, written and designed by:
        Frédo Durand
        Katherine L. Bouman
        Gaurav Chaurasia
        Adrian Vasile Dalca
        Neal Wadhwa

    With additions & modifications by
        Wojciech Jarosz
    for Dartmouth's CS 89/189.
*/
#pragma once

#include "floatimage.h"

// A6 Functions: Scaling and Rotating an Image
FloatImage scale(const FloatImage &im, int x, int y);
FloatImage scaleNN(const FloatImage &im, float factor);
FloatImage scaleLin(const FloatImage &im, float factor);
FloatImage scaleCubic(const FloatImage &im, float factor);
float interpolateLin(const FloatImage &im, float x, float y, int z, bool clamp=false);
float interpolateBicubic(const FloatImage &im, float x, float y, int z, bool clamp=false);
float bicubic(float p0, float p1, float p2, float p3, float t);
FloatImage rotate(const FloatImage &im, float theta);