#ifndef ALPHA_BLENDING
#define ALPHA_BLENDING

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <SFML/Graphics.hpp>

const size_t WIDTH = 800;
const size_t HEIGTH = 600;

const int IMP_X = 280;
const int IMP_Y = 220;

struct pixel_image {    
    sf::Image        image   ;
    const sf::Uint8* pixels  ;
    int              width   ;
    int              height  ;
    int              n_pixels;
};

class FPS
{
    private:
        sf::Clock clock_; 
        unsigned int frame_;
        float fps_;

    public:
        FPS() : frame_(0), fps_(0) {}
       ~FPS() {frame_ = 0; fps_ = 0;}

        float getFPS() const {return fps_;}

        void update()
        {
            if (clock_.getElapsedTime().asSeconds() >= 1.f)
            {
                fps_   = frame_ / clock_.getElapsedTime().asSeconds();
                frame_ = 0;

                clock_.restart();                
            }

            ++frame_;
        }
};

void alpha_blending (const char* name_picture_back, const char* name_picture_front, 
                     void (*set_alpha_blending) (const sf::Color*, const sf::Color*, sf::Color*, const int));

inline void allign_pixel_image (struct pixel_image* alligned_img, struct pixel_image* source_img, int x, int y);

inline void pixel_image_ctor(struct pixel_image* img, const char* img_name);

inline void pixel_image_create_from_pixels (volatile struct pixel_image* img, int width, int height, const sf::Uint8* pixels);

inline int read_file(FILE* file, sf::Uint8** buf);

#endif //ALPHA_BLENDING