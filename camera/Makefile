CFLAGS=`pkg-config --cflags glew glfw3 assimp`
LIBS= `pkg-config --libs assimp` -L/usr/local/lib -lGLEW -lGLU -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
all:
	g++ -g $(CFLAGS) camera.cc -o camera $(LIBS)
