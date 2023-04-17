#include "alpha_blending_avx.hpp"

void set_alpha_blending_avx (const sf::Color* Back, const sf::Color* Front, sf::Color* Screen, const int n_pixels)
{
    for (int i = 0; i < n_pixels; i += 8)
    {
        //================================================================================
        // front128.pixelL 
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // fr1  fg1  fb1  fa1  fr2  fg2  fb2  fa2   
        // front128.pixelH
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // fr3  fg3  fb3  fa3  fr4  fg4  fb4  fa4   00   00   00   00   00   00   00   00
        // back128.pixelL
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // br1  bg1  bb1  ba1  br2  bg2  bb2  ba2   
        // back128.pixelH
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // br3  bg3  bb3  ba3  br4  bg4  bb4  ba4   00   00   00   00   00   00   00   00
        //================================================================================
        __m128pixel front128 = { _mm_load_si128((__m128i*)(Front + i)),
                                 _mm_load_si128((__m128i*)(Front + i + 4))};
        __m128pixel back128 = {_mm_load_si128((__m128i*)(Back + i)),
                               _mm_load_si128((__m128i*)(Back + i + 4))};

        //================================================================================
        // front.pixelL 
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // fr1   00  fg1   00  fb1   00  fa1   00  fr2   00  fg2   00  fb2   00  fa2   00
        // front.pixelH
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // fr3   00  fg3   00  fb3   00  fa3   00  fr4   00  fg4   00  fb4   00  fa4   00
        // back.pixelL
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // br1   00  bg1   00  bb1   00  ba1   00  br2   00  bg2   00  bb2   00  ba2   00
        // back.pixelH
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // br3   00  bg3   00  bb3   00  ba3   00  br4   00  bg4   00  bb4   00  ba4   00
        //================================================================================
        __m256pixel front = {_mm256_cvtepu8_epi16(front128.pixelL),
                             _mm256_cvtepu8_epi16(front128.pixelH)};
        __m256pixel back  = {_mm256_cvtepu8_epi16(back128.pixelL),
                             _mm256_cvtepu8_epi16(back128.pixelH)};

        //__m256pixel a = {_mm256_cvtepu8_epi16(a128.pixelL), _mm256_cvtepu8_epi16(a128.pixelH)};

        //================================================================================
        // alpha.pixelL
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // fa1   00  fa1   00  fa1   00  fa1   00  fa2   00  fa2   00  fa2   00  fa2   00
        // alpha.pixelH
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // fa3   00  fa3   00  fa3   00  fa3   00  fa4   00  fa4   00  fa4   00  fa4   00
        //================================================================================
        __m256i alpha_shuffle_mask = _mm256_set_epi8(ZERO, 30, ZERO, 30, ZERO, 30, ZERO, 30,
                                                     ZERO, 22, ZERO, 22, ZERO, 22, ZERO, 22,
                                                     ZERO, 14, ZERO, 14, ZERO, 14, ZERO, 14,
                                                     ZERO, 6 , ZERO, 6 , ZERO, 6 , ZERO, 6);
        __m256pixel alpha = {_mm256_shuffle_epi8(front.pixelL, alpha_shuffle_mask),
                             _mm256_shuffle_epi8(front.pixelH, alpha_shuffle_mask)};

        //========================================================================================
        // front.pixelL
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // fr1 * a1  fg1 * a1  fb1 * a1  fa1 * a1  fr2 * a2  fg2 * a2  fb2 * a2  fa2 * a2
        // front.pixelH
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // fr3 * a3  fg3 * a3  fb3 * a3  fa3 * a3  fr4 * a4  fg4 * a4  fb4 * a4  fa4 * a4
        // back.pixelL
        // [15] [14]  [13] [12]  [11] [10]  [09] [08]  [07] [06]  [05] [04]  [03] [02]  [01] [00]
        // br1*(1-a1) bg1*(1-a1) bb1*(1-a1) ba1*(1-a1) br2*(1-a2) bg2*(1-a2) bb2*(1-a2) ba2*(1-a2)
        // back.pixelH
        // [15] [14]  [13] [12]  [11] [10]  [09] [08]  [07] [06]  [05] [04]  [03] [02]  [01] [00]
        // br3*(1-a3) bg3*(1-a3) bb3*(1-a3) ba3*(1-a3) br4*(1-a4) bg4*(1-a4) bb4*(1-a4) ba4*(1-a4)
        //========================================================================================
        front.pixelL = _mm256_mullo_epi16(front.pixelL, alpha.pixelL);                             
        front.pixelH = _mm256_mullo_epi16(front.pixelH, alpha.pixelH); 

        back.pixelL = _mm256_mullo_epi16(back.pixelL, _mm256_sub_epi16(_mm256_set1_epi16(255), alpha.pixelL));  
        back.pixelH = _mm256_mullo_epi16(back.pixelH, _mm256_sub_epi16(_mm256_set1_epi16(255), alpha.pixelH));

        //================================================================================
        // sum.pixelL
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // sr1       sg1       sb1       sa1       sr2       sg2       sb2       sa2
        // sum.pixelH
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // sr3       sg3       sb3       sa3       sr4       sg4       sb4       sa4
        //================================================================================
        __m256pixel sum = {_mm256_add_epi16(front.pixelL, back.pixelL),  
                           _mm256_add_epi16(front.pixelH, back.pixelH)};

        //=======================================================================================================
        // sum.pixelL
        //  [15]   [14]   [13]   [12]   [11]   [10]   [09]   [08]  [07]  [06]  [05]  [04]  [03]  [02]  [01]  [00]
        // sr1>>8 sg1>>8 sb1>>8 sa1>>8 sr2>>8 sg2>>8 sb2>>8 sa2>>8  ??    ??    ??    ??    ??    ??    ??    ??
        // sum.pixelH
        //  [15]   [14]   [13]   [12]   [11]   [10]   [09]   [08]  [07]  [06]  [05]  [04]  [03]  [02]  [01]  [00]
        // sr3>>8 sg3>>8 sb3>>8 sa3>>8 sr4>>8 sg4>>8 sb4>>8 sa4>>8  ??    ??    ??    ??    ??    ??    ??    ??
        //=======================================================================================================
        __m256i sum_shuffle_mask = _mm256_set_epi8(ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO,
                                                   ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO,
                                                   31,   29,   27,   25,   23,   21,   19,   17,
                                                   15,   13,   11,   9,    7,    5,    3 ,   1);
        sum.pixelL = _mm256_shuffle_epi8(sum.pixelL, sum_shuffle_mask);    
        sum.pixelH = _mm256_shuffle_epi8(sum.pixelH, sum_shuffle_mask);

        //================================================================================
        // screen
        // [15] [14] [13] [12] [11] [10] [09] [08] [07] [06] [05] [04] [03] [02] [01] [00]
        // sr1  sg1  sb1  sa1  sr2  sg2  sb2  sa2  sr3  sg3  sb3  sa3  sr4  sg4  sb4  sa4
        //================================================================================
        __m128pixel screen = {_mm256_castsi256_si128(sum.pixelL),
                              _mm256_castsi256_si128(sum.pixelH)};

        _mm_store_si128((__m128i*)(Screen + i), screen.pixelL);
        _mm_store_si128((__m128i*)(Screen + i + 4), screen.pixelH);

    }
}