#ifndef ALPHA_BLENDING_AVX
#define ALPHA_BLENDING_AVX

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <immintrin.h>

#include <SFML/Graphics.hpp>

const int ZERO = 256;

struct __m128pixel {
    __m128i pixelL;
    __m128i pixelH;
};

struct __m256pixel {
    __m256i pixelL;
    __m256i pixelH;
};

void set_alpha_blending_avx (const sf::Color* Back, const sf::Color* Front, sf::Color* Screen, const int n_pixels);

#endif // ALPHA_BLENDING_AVX