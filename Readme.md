# Оптимизация алгоритма альфа-блендинга
## О ПРОЕКТЕ

**Alpha Blending** - это процесс комбинирования двух изображений на экране с учётом их альфа-каналов. Альфа-канал характеризует прозрачность изображения. В данной работе мы будем накладывать одно изображение на другое.

## Запуск программы

Прежде всего, скачайте этот репозиторий:
```bash
git@github.com:KirillLakhnov/Alpha-blending
```
Cкомпилируйте программу с помощью **makefile**:
```bash
make no_o   \\ для версии без оптимизаций
make sse    \\ для версии с SSE
```
Запустите программу:
```bash
./a.out
```

Примечание: Эта программа требует, чтобы SFML был установлен в вашей системе. Вы можете скачать SFML с официального сайта: https://www.sfml-dev.org/download.php

## Алгоритм альфа-блендинга

Этот алгоритм широко используется для объединения изображений. Есть два снимка: задний и передний. Второй должен быть наложен на первый. 

### Пример работы алгоритма:
![Rezult](readme_files/AlphaBlendingPicture.png)

Чтобы объединить изображения одинакового размера, мы используем алгоритм, который берет каждый пиксель обоих изображений (массивы спереди и сзади) и вычисляет пиксели результирующего изображения (массив экрана) с помощью простых инструментов:

``color = (front_color * front_alpha + back_color * (255 - front_alpha) >> 8)``

#### Код неоптимизированного алгоритма
```C++
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
```

Производительность алгоритма без оптимизаций пропорциональна размеру изображения.

Теперь оптимизируем данный алгоритм с помощью набора команд SSE4 из библиотеки <nmmintrin.h>. 

#### Код оптимизированного алгоритма
```C++
for (int i = 0; i < n_pixels; i += 4)
{
    __m128pixel front = {_mm_load_si128((__m128i*)(Front + i)),                                     // front.pixelL = [fr1  fg1  fb1  fa1  fr2  fg2  fb2  fa2 ?? ?? ?? ?? ?? ?? ?? ?? ]
                        (__m128i) _mm_movehl_ps((__m128) _mm_set1_epi8(0), (__m128) front.pixelL)}; // front.pixelH = [fr3  fg3  fb3  fa3  fr4  fg4  fb4  fa4   00   00   00   00   00   00   00   00]
    __m128pixel back = {_mm_load_si128((__m128i*)(Back + i)),                                       // back.pixelL = [br1  bg1  bb1  ba1  br2  bg2  bb2  ba2 ?? ?? ?? ?? ?? ?? ?? ??]
                       (__m128i) _mm_movehl_ps((__m128) _mm_set1_epi8(0), (__m128) back.pixelL)};   // back.pixelH = [br3  bg3  bb3  ba3  br4  bg4  bb4  ba4   00   00   00   00   00   00   00   00]

    front.pixelL = _mm_cvtepu8_epi16(front.pixelL); // front.pixelL = [fr1   00  fg1   00  fb1   00  fa1   00  fr2   00  fg2   00  fb2   00  fa2   00]
    front.pixelH = _mm_cvtepu8_epi16(front.pixelH); // front.pixelH = [fr3   00  fg3   00  fb3   00  fa3   00  fr4   00  fg4   00  fb4   00  fa4   00]

    back.pixelL = _mm_cvtepu8_epi16(back.pixelL); // back.pixelL = [br1   00  bg1   00  bb1   00  ba1   00  br2   00  bg2   00  bb2   00  ba2   00]
    back.pixelH = _mm_cvtepu8_epi16(back.pixelH); // back.pixelH = [br3   00  bg3   00  bb3   00  ba3   00  br4   00  bg4   00  bb4   00  ba4   00]

    __m128i alpha_shuffle_mask = _mm_set_epi8(ZERO, 14, ZERO, 14, 
                                              ZERO, 14, ZERO, 14,
                                              ZERO, 6,  ZERO, 6, 
                                              ZERO, 6,  ZERO, 6);
    __m128pixel alpha = {_mm_shuffle_epi8(front.pixelL, alpha_shuffle_mask),       // alpha.pixelL = [fa1   00  fa1   00  fa1   00  fa1   00  fa2   00  fa2   00  fa2   00  fa2   00]
                         _mm_shuffle_epi8(front.pixelH, alpha_shuffle_mask)};      // alpha.pixelH = [fa3   00  fa3   00  fa3   00  fa3   00  fa4   00  fa4   00  fa4   00  fa4   00]

    front.pixelL = _mm_mullo_epi16(front.pixelL, alpha.pixelL); // front.pixelL = [fr1 * a1  fg1 * a1  fb1 * a1  fa1 * a1  fr2 * a2  fg2 * a2  fb2 * a2  fa2 * a2]
    front.pixelH = _mm_mullo_epi16(front.pixelH, alpha.pixelH); // front.pixelH = [fr3 * a3  fg3 * a3  fb3 * a3  fa3 * a3  fr4 * a4  fg4 * a4  fb4 * a4  fa4 * a4]

    back.pixelL = _mm_mullo_epi16(back.pixelL, _mm_sub_epi16(_mm_set1_epi16(255), alpha.pixelL));   // back.pixelL = [br1*(1-a1) bg1*(1-a1) bb1*(1-a1) ba1*(1-a1) br2*(1-a2) bg2*(1-a2) bb2*(1-a2) ba2*(1-a2)]
    back.pixelH = _mm_mullo_epi16(back.pixelH, _mm_sub_epi16(_mm_set1_epi16(255), alpha.pixelH));   // back.pixelH = [br3*(1-a3) bg3*(1-a3) bb3*(1-a3) ba3*(1-a3) br4*(1-a4) bg4*(1-a4) bb4*(1-a4) ba4*(1-a4)]

     __m128pixel sum = {_mm_add_epi16(front.pixelL, back.pixelL),   // sum.pixelL = [sr1       sg1       sb1       sa1       sr2       sg2       sb2       sa2   ]
                        _mm_add_epi16(front.pixelH, back.pixelH)};  // sum.pixelH = [sr3       sg3       sb3       sa3       sr4       sg4       sb4       sa4    ]

    __m128i sum_shuffle_mask = _mm_set_epi8(ZERO, ZERO, ZERO, ZERO, 
                                            ZERO, ZERO, ZERO, ZERO,
                                            15,   13,   11,   9,     
                                            7,    5,    3,    1);

    sum.pixelL = _mm_shuffle_epi8(sum.pixelL, sum_shuffle_mask); // sum.pixelL = [sr1>>8 sg1>>8 sb1>>8 sa1>>8 sr2>>8 sg2>>8 sb2>>8 sa2>>8  ??    ??    ??    ??    ??    ??    ??    ??]
    sum.pixelH = _mm_shuffle_epi8(sum.pixelH, sum_shuffle_mask); // sum.pixelH = [sr3>>8 sg3>>8 sb3>>8 sa3>>8 sr4>>8 sg4>>8 sb4>>8 sa4>>8  ??    ??    ??    ??    ??    ??    ??    ??]

    __m128i screen = (__m128i) _mm_movelh_ps((__m128) sum.pixelL , (__m128) sum.pixelH); // screen = [sr1  sg1  sb1  sa1  sr2  sg2  sb2  sa2  sr3  sg3  sb3  sa3  sr4  sg4  sb4  sa4]
        
    _mm_store_si128((__m128i*)(Screen + i), screen);
}
```

## Результаты оптимизаций

В таблице ниже приведено среднее время расчетов для различных типов оптимизаций.

|Оптимизаиция \ Флаг|НЕТ      |-O1     |-O2      |-O3      |-Ofast   |
|:------------------|:-------:|:------:|:-------:|:-------:|:-------:|
|БЕЗ оптимизаций    |0.0106 c |0.0081 с|0.0080 с |0.0079 с |0.0081 c |
|                   |         |        |         |         |         |
|SSE инструкции     |0.0129 c |0.0068 c|0.0069 c |0.0069 c |0.0068 c |    

Время выполнения программы с использованием SIMD-инструкций без указания флага оптимизации больше, чем без них. Это связано с неэффективным использованием ``xmm`` регистров. В этом можно убедиться, просмотрев ассемблерный код фрагмента нашей программы с помощью сервиса https://godbolt.org/.

### Ассемблерный код функции
![asm_func](readme_files/ASM_function.png)

Разница скорости версии без SSE и флагов и самой быстрой версии равна **0.0106/0.0068 = 1.6** раз.
Разница скорости самой медленной версии и самой быстрой - **0.0129/0.0068 = 1.9** раз.

## Вывод

В ходе данной работы были исследованы возможности процессора оптимизации программ с использованием принципа SIMD. Были получены результаты прироста скорости в 1.6 - 1.9 раз. Это показывает, что принцип параллелизма позволяет достичь огромного прироста в производительности.

## Источники и литератрура 

1. SIMD:
    - https://ru.wikipedia.org/wiki/SIMD
    - https://www.laruence.com/sse

2. Alpha Blending:
    - https://ru.wikipedia.org/wiki/Множество_Мандельброта

3. SFML:
    - https://www.sfml-dev.org/
    - https://ru.wikipedia.org/wiki/SFML
    - https://ps-group.github.io/ps1/lesson_sfml_1

