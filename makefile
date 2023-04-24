O_FLAGS := -Ofast

no_o: main.cpp alpha_blending.cpp alpha_blending_no_o.cpp
	g++ main.cpp alpha_blending.cpp alpha_blending_no_o.cpp -lsfml-graphics -lsfml-window -lsfml-system $(O_FLAGS) -DNO_O

sse: main.cpp alpha_blending.cpp alpha_blending_sse.cpp
	g++ main.cpp alpha_blending.cpp alpha_blending_sse.cpp -lsfml-graphics -lsfml-window -lsfml-system $(O_FLAGS) -DSSE