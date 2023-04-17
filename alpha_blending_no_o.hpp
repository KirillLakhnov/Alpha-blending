#ifndef ALPHA_BLENDING_NO_O
#define ALPHA_BLENDING_NO_O

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <SFML/Graphics.hpp>

void set_alpha_blending_no_o (const sf::Color* Back, const sf::Color* Front, sf::Color* Screen, const int n_pixels);

#endif // ALPHA_BLENDING_NO_O