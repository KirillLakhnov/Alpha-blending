//#define NO_O
//#define SSE
//#define AVX

#include "alpha_blending.hpp"

#ifdef NO_O
    #include "alpha_blending_no_o.hpp"
#endif

#ifdef SSE
    #include "alpha_blending_sse.hpp"
#endif

#ifdef AVX
    #include "alpha_blending_avx.hpp"
#endif

int main ()
{
    const char  name_back_picture[] = "picture/Table.bmp";
    const char name_front_picture[] = "picture/AskhatCat.bmp";

#ifdef NO_O
    alpha_blending (name_back_picture, name_front_picture, set_alpha_blending_no_o);
#endif

#ifdef SSE
    alpha_blending (name_back_picture, name_front_picture, set_alpha_blending_sse);
#endif

#ifdef AVX
    alpha_blending (name_back_picture, name_front_picture, set_alpha_blending_avx);
#endif

    return 0;
}