CC	= x86_64-w64-mingw32-g++
CFLAGS	= -Wall
LFLAGS	= -static-libgcc -static-libstdc++
TARGET	= osu-pause.exe 

64:	CC = x86_64-w64-mingw32-g++
64:	$(TARGET)

32:	CC = i686-w64-mingw32-g++
32:	$(TARGET)

$(TARGET):
	$(CC) main.cpp -o $@ $(CFLAGS) $(LFLAGS)

