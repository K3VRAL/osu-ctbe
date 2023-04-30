CC	= x86_64-w64-mingw32-g++
CFLAGS	= -Wall -c -Iinclude/
LFLAGS	= -static-libgcc -static-libstdc++
TARGET	= osu-pause.exe 
BINFLR	= bin/

win64:	CC = x86_64-w64-mingw32-g++
win64:	$(TARGET)

win32:	CC = i686-w64-mingw32-g++
win32:	$(TARGET)

# Compiling specific object
%.o: %.cpp | $(BINFLR)
	$(CC) -o $(BINFLR)$(notdir $@) $< $(CFLAGS)

# Compiling all objects to make an executable
$(TARGET): $(addsuffix .o, $(basename $(shell find "src/" -type "f" -name "*.cpp")))
	$(CC) -o $(BINFLR)$@ $(addprefix $(BINFLR), $(notdir $^)) $(LFLAGS)

# Make bin/ folder
$(BINFLR):
	$(shell mkdir -p $@)

# Clean up
clean:
	$(shell rm -rf $(BINFLR))
