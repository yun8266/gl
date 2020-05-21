CFLAGS=`pkg-config --cflags glew glfw3`
LIBS= -L/usr/local/lib -lGLEW -lGLU -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
all:
	g++ -g $(CFLAGS) rotating.cc -o rotating $(LIBS)
