CC = gcc
CFLAGS = -Wall -Werror

# 静态链接库
STATIC_LIB = lib/libfunctions.a

CC = gcc
CFLAGS = -Wall -Werror

# 静态链接库
STATIC_LIB = lib/libfunctions.a

# 动态链接库
DYNAMIC_LIB = lib/libfunctions.so

# 所有源文件
SRCS = lib/define.h lib/function.h lib/function.c test/main.c

# 目标文件
OBJS = lib/function.o test/main.o

all: $(STATIC_LIB) $(DYNAMIC_LIB) main

$(STATIC_LIB): lib/function.o
	ar rcs $(STATIC_LIB) lib/function.o

$(DYNAMIC_LIB): lib/function.o
	$(CC) -shared -o $(DYNAMIC_LIB) lib/function.o

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o main

lib/function.o: lib/function.c lib/function.h lib/define.h
	$(CC) $(CFLAGS) -c lib/function.c -o lib/function.o

test/main.o: test/main.c lib/function.h lib/define.h
	$(CC) $(CFLAGS) -c test/main.c -o test/main.o

clean:
	rm -f $(OBJS) main $(STATIC_LIB) $(DYNAMIC_LIB)
