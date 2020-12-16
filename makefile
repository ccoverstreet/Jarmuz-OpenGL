driver: main.cpp src/index_buffer.cpp src/vertex_buffer.cpp
	g++ main.cpp src/index_buffer.cpp src/vertex_buffer.cpp -o driver -lGL -lGLEW -lglfw -lm

run: driver
	./driver
