
a.out: Entity.cpp GameEngine.cpp Level.cpp main.cpp
	g++ -std=c++11 Entity.cpp GameEngine.cpp Level.cpp main.cpp -lSDL2 -lSDL2_Image

