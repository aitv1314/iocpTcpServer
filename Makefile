CC = x86_64-w64-mingw32-gcc-posix
XX = x86_64-w64-mingw32-g++-posix

FLAGS = -lws2_32 -D_WIN32_WINNT=0x0A00  -std=c++17

SRCS = ./*.cpp
TARGET = iocp.exe

all:
	$(XX) $(SRCS) -o $(TARGET) $(FLAGS)