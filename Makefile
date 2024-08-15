CC = x86_64-w64-mingw32-gcc-posix
XX = x86_64-w64-mingw32-g++-posix

GCC_FLAGS = -g -O3 -std=c++17
LIB_FLAGS = -lws2_32
USR_FLAGS = -D_WIN32_WINNT=0x0A00

# -Q --help=optimizers
FLAGS = $(LIB_FLAGS) $(USR_FLAGS)  $(GCC_FLAGS)

SRCS = ./*.cpp
TARGET = iocp.exe

all:
	$(XX) $(SRCS) -o $(TARGET) $(FLAGS) 

cpi:
	$(XX) $(SRCS) -o $(TARGET) $(FLAGS) -Q --help=optimizers