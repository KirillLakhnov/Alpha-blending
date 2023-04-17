#include "alpha_blending_no_o.hpp"

void set_alpha_blending_no_o (const sf::Color* Back, const sf::Color* Front, sf::Color* Screen, const int n_pixels)
{
    // color = front_color * front_alpha + back_color * (255 - front_alpha) >> 8

    for (int i = 0; i < n_pixels; i++)
    {
        sf::Color front_color = Front[i];
        sf::Color back_color = Back[i];
        sf::Uint8 a = front_color.a;

        Screen[i].r = ((sf::Uint8)(((front_color.r) * a + back_color.r * (255 - a)) >> 8));
        Screen[i].g = ((sf::Uint8)(((front_color.g) * a + back_color.g * (255 - a)) >> 8));
        Screen[i].b = ((sf::Uint8)(((front_color.b) * a + back_color.b * (255 - a)) >> 8));
        Screen[i].a = ((sf::Uint8)(((front_color.a) * a + back_color.a * (255 - a)) >> 8));
    }
}