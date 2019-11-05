# 1、准备工作，编译方式、目标文件名、依赖库路径的定义。

#export CC=/home/jasonlai001/workspace/Toolchain/buildroot_gcc/usr/bin/mipsel-linux-gcc-4.6.4
#CC = mipsel-linux-gcc-4.6.4
export CC = /home/jason.lai/workspace/AIA/3308_dev/buildroot/output/rockchip_rk3308_32_dueros/host/bin/arm-rockchip-linux-gnueabihf-g++
CFLAGS  := -Wall -O3 -std=c++0x -D_USE_LINUX  


CUR_DIR = $(shell pwd)

OS_SRC_DIR=${CUR_DIR}/os/src
OTA_SRC_DIR=${CUR_DIR}/skyota/src
UTL_SRC_DIR=${CUR_DIR}/util/src

SRC = ${wildcard  ${OS_SRC_DIR}/*.cpp} \
      ${wildcard  ${OTA_SRC_DIR}/*.cpp} \
      ${wildcard  ${UTL_SRC_DIR}/*.cpp} \

OBJECTS = ${patsubst %.cpp, %.o, ${SRC}}
INCLUDE_DIRS = -I./include/curl -I./include -I./include/os -I./os/include -I./skyota/include -I./util/include


# opencv 头文件和lib路径 
OPENCV_INC_ROOT = /usr/local/include/opencv 
OPENCV_LIB_ROOT = ${CUR_DIR}/libs

OBJS = $(OBJECTS) #.o文件与.cpp文件同名
LIB = libOtaUpgrade.so # 目标文件名 

OPENCV_INC= -I $(OPENCV_INC_ROOT)

INCLUDE_PATH = $(INCLUDE_DIRS)

LIB_PATH = -L $(OPENCV_LIB_ROOT)

# 依赖的lib名称
OPENCV_LIB = -lcurl -lpthread -lrt

all : $(LIB)

# 2. 生成.o文件 
%.o : %.cpp
	$(CC) $(CFLAGS) -fpic -c $< -o $@ $(INCLUDE_PATH) $(LIB_PATH) $(OPENCV_LIB) 

# 3. 生成动态库文件
$(LIB) : $(OBJS)
	rm -f $@
	$(CC) -shared -o $@ $(OBJS)
	rm -f $(OBJS)
	@echo "Build successfully"

tags :
	ctags -R *

# 4. 删除中间过程生成的文件 
clean:
	rm -f $(OBJS) $(TARGET) $(LIB)