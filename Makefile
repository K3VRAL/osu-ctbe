CC	= x86_64-w64-mingw32-g++
CFLAGS	= -Wall
LFLAGS	= -static-libgcc -static-libstdc++
TARGET	= osu-pause.exe 

$(TARGET):
	$(CC) main.cpp -o $@ $(CFLAGS) $(LFLAGS)

