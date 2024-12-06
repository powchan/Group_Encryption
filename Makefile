# Makefile for main.c with AES.h and DES.h dependencies

# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -I./  # -I./ 用于包含当前目录中的头文件

# 链接选项
LDFLAGS =

# 输出的目标程序名称
TARGET = main

# 源代码文件
SRC = main.c

# 生成的目标程序
$(TARGET): $(SRC) AES.h DES.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# 清理规则
clean:
	rm -f $(TARGET)

# 伪目标
.PHONY: clean
