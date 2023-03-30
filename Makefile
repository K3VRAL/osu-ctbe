CC	= x86_64-w64-mingw32-g++
CFLAGS	= -Wall
LFLAGS	= -static-libgcc -static-libstdc++
TARGET	= osu-pause.exe 

all:	$(TARGET)

win64:	CC = x86_64-w64-mingw32-g++
win64:	$(TARGET)

win32:	CC = i686-w64-mingw32-g++
win32:	$(TARGET)

$(TARGET):
	$(CC) main.cpp -o $@ $(CFLAGS) $(LFLAGS)

