O_FLAGS := -Ofast
NO_DRAW_MODE := -DNO_DRAW_MODE

no_o: main.cpp alpha_blending.cpp alpha_blending_no_o.cpp
	g++ main.cpp alpha_blending.cpp alpha_blending_no_o.cpp -lsfml-graphics -lsfml-window -lsfml-system $(O_FLAGS) $(NO_DRAW_MODE) -DNO_O

sse: main.cpp alpha_blending.cpp alpha_blending_sse.cpp
	g++ main.cpp alpha_blending.cpp alpha_blending_sse.cpp -lsfml-graphics -lsfml-window -lsfml-system $(O_FLAGS) $(NO_DRAW_MODE) -DSSE