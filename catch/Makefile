CFLAGS=`pkg-config --cflags glew glfw3 assimp freetype2`
LIBS= `pkg-config --libs assimp freetype2` -L/usr/local/lib -lGLEW -lGLU -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
all:
	g++ -g $(CFLAGS) catch.cc -o catch $(LIBS)
