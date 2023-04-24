//#define SFML

#include "alpha_blending.hpp"

void alpha_blending (const char* name_picture_back, const char* name_picture_front, 
                     void (*set_alpha_blending) (const sf::Color*, const sf::Color*, sf::Color*, const int))
{
    assert (name_picture_back);
    assert (name_picture_front);

    sf::RenderWindow window (sf::VideoMode(WIDTH, HEIGTH), "Alpha blending");

    struct pixel_image table_img = {};
    pixel_image_ctor (&table_img, name_picture_back);

    struct pixel_image cat_img = {};
    pixel_image_ctor (&cat_img, name_picture_front);

    sf::Uint8* cat_alligned_img_pixels = (sf::Uint8*) calloc (table_img.width*table_img.height*4, sizeof (sf::Uint8)); 
    struct pixel_image cat_alligned_img = {};
    pixel_image_create_from_pixels (&cat_alligned_img, table_img.width, table_img.height, cat_alligned_img_pixels);
    
    sf::Uint8* screen_img_pixels = (sf::Uint8*) calloc (table_img.width*table_img.height*4, sizeof (sf::Uint8));
    volatile struct pixel_image screen_img = {};
    pixel_image_create_from_pixels (&screen_img, table_img.width, table_img.height, screen_img_pixels);

    allign_pixel_image (&cat_alligned_img, &cat_img, IMP_X, IMP_Y);

    sf::Texture texture;
    texture.loadFromImage (table_img.image);
    sf::Sprite sprite (texture);

    sf::Font font;
    font.loadFromFile ("sfml_files/SrbijaSans.otf");

    sf::Text text_fps;
    text_fps.setFont (font);
    text_fps.setPosition (25, 25);
    text_fps.setCharacterSize (40);
    text_fps.setColor (sf::Color (20, 250, 20));
    text_fps.setStyle (sf::Text::Bold);

    FPS fps;
    char fps_buffer[10];

    while (window.isOpen())
    {
        sf::Event event = {};

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        fps.update ();

        set_alpha_blending ((sf::Color*) table_img.pixels, (sf::Color*) cat_alligned_img.pixels, (sf::Color*) screen_img.pixels, table_img.n_pixels);

        sprintf(fps_buffer, "%.2f", fps.getFPS());
        sf::String str(fps_buffer);
        text_fps.setString(str);

        texture.update(screen_img.pixels, screen_img.width, screen_img.height, 0, 0);
        sprite.setTexture(texture);

        window.clear();
        window.draw(sprite);
        window.draw(text_fps);
        window.display();
    }
}

void allign_pixel_image (struct pixel_image* alligned_img, struct pixel_image* source_img, int x, int y)
{
    assert (alligned_img);
    assert (source_img);

    sf::Color* alligned_pixels = (sf::Color*) alligned_img->pixels;
    sf::Color* source_pixels = (sf::Color*) source_img->pixels;

    const int max_yi = y + source_img->height - 1;
    const int max_xi = x + source_img->width  - 1;

    int alligned_pixel_i = y * alligned_img->width + x;
    int source_pixel_i = 0;

    for (int yi = y - 1; yi < max_yi; yi++)
    {
        for (int xi = x - 1; xi < max_xi; xi++, alligned_pixel_i++, source_pixel_i++)
        {
            alligned_pixels[alligned_pixel_i] = source_pixels[source_pixel_i];
        }

        alligned_pixel_i += alligned_img->width - source_img->width;
    }
}

void pixel_image_ctor(struct pixel_image* img, const char* img_name)
{
	assert (img_name);
    assert (img);

	img->image.loadFromFile(img_name);
	img->pixels	  = img->image.getPixelsPtr();
	img->width    = img->image.getSize().x;
	img->height   = img->image.getSize().y;
	img->n_pixels = img->width * img->height;
}

void pixel_image_create_from_pixels (volatile struct pixel_image* img, int width, int height, const sf::Uint8* pixels)
{
    assert (pixels);
    assert (img);

    img->height   = height;
    img->width    = width;
    img->n_pixels = width*height;
    img->pixels   = pixels;
}

int read_file(FILE* file, sf::Uint8** buf)
{
    assert (buf);
    assert (file);

    fseek(file, 0L, SEEK_END);
    size_t file_len = ftell(file);
    fseek(file, 0L, SEEK_SET);

    return fread (*buf, sizeof(sf::Uint8), file_len, file);
}

