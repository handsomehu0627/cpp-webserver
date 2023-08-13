CXX = g++

# -O2 编译优化type2 -Wall 打印异常告警信息 -g 添加GDB调试信息
CFLAGS = -std=c++14 -O2 -Wall -g

# 生成的executable名字叫server
TARGET = server

OBJS = ../code/log/*.cpp ../code/pool/*.cpp ../code/timer/*.cpp \
       ../code/http/*.cpp ../code/server/*.cpp \
       ../code/buffer/*.cpp ../code/main.cpp

# make的时候默认执行all下的命令
all: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -o ../bin/$(TARGET)  -pthread

# make clean 才会执行clean下的命令
clean:
	rm -rf ../bin/$(OBJS) $(TARGET)
