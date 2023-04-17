#ifndef ALPHA_BLENDING_SSE
#define ALPHA_BLENDING_SSE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <nmmintrin.h>

#include <SFML/Graphics.hpp>

const int ZERO = 128;

struct __m128pixel {
    __m128i pixelL;
    __m128i pixelH;
};

void set_alpha_blending_sse (const sf::Color* Back, const sf::Color* Front, sf::Color* Screen, const int n_pixels);

#endif // ALPHA_BLENDING_SSE