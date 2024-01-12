##################################### 工程内容 ##################################
# := 复制赋值
TOPDIR		  := .
#$(shell pwd)

SRC_DIR  	  := $(TOPDIR)/src
INC_DIR 	  := $(TOPDIR)/include
LIB_DIR 	  := $(TOPDIR)/lib
OUT_DIR	 	  := $(TOPDIR)/out
PROJ_DIR 	  := $(TOPDIR)/proj

# 查找SRC_DIR内的所有*.cpp
SRC_CPP_FILES:= $(wildcard  $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp)
SRC_FILES 	  := $(wildcard $(SRC_DIR)/*.c)

# 从$(SRC_FILES)将 “$(SRC_DIR)/%.cpp” 格式转换为 “$(SRC_DIR)/%.o”
OBJ_CPP_FILES:= $(patsubst $(SRC_DIR)/%.cpp, $(PROJ_DIR)/%.o, $(SRC_CPP_FILES))
OBJ_FILES 	  := $(patsubst $(SRC_DIR)/%.c, $(PROJ_DIR)/%.o, $(SRC_FILES))
#OBJS		  := $(OBJ_CPP_FILES) $(OBJ_FILES)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(PROJ_DIR)/%.o,$(patsubst $(SRC_DIR)/%.c,$(PROJ_DIR)/%.o,$(SRC_CPP_FILES)))


##################################### 工程目标 ##################################
TRAGET  	  := $(OUT_DIR)/v4l2_ffmpeg

#打印
$(info "=============START=============")
$(info $(OBJS))
$(info "=============END=============")

# static shard
LINK_TYPE := static

##################################### 编译链 ##################################
TOOL_PATH	  = arm-linux-gnueabihf-
CROSS_COMPILE = $(TOOL_PATH)

##################################### 用户设定 ##################################
AS        = $(CROSS_COMPILE)as		#
LD        = $(CROSS_COMPILE)ld		#链接工具
CC        = $(CROSS_COMPILE)gcc		#编译工具
CPP		  = $(CROSS_COMPILE)g++		#
AR        = $(CROSS_COMPILE)ar		#打包工具
NM        = $(CROSS_COMPILE)nm		#

STRIP     = $(CROSS_COMPILE)strip	#优化工具
OBJCOPY   = $(CROSS_COMPILE)objcopy #
OBJDUMP   = $(CROSS_COMPILE)objdump #

export AS LD CC CPP AR NM           #将定义的变量导出，方便其他makefile使用
export STRIP OBJCOPY OBJDUMP        #将定义的变量导出，方便其他makefile使用


##################################### 头文件 #####################################
INC	:=  -I$(TOPDIR)/include
#INC +=  -I$(TOPDIR)/include/libavcodec	\
#		-I$(TOPDIR)/include/libavdevice \
#		-I$(TOPDIR)/include/libavfilter	\
#		-I$(TOPDIR)/include/libavformat \
#		-I$(TOPDIR)/include/libavutil	\
#		-I$(TOPDIR)/include/libpostproc \
#		-I$(TOPDIR)/include/libswresample \
#		-I$(TOPDIR)/include/libswscale	
#INC	+=  -I/home/risun/linux/linux/include		\
#		-I/home/risun/linux/linux/include/linux \
#		-I/home/risun/linux/linux/include/uapi

##################################### 链接库 #####################################
#-L $(TOPDIR)/lib/shard
#LIBS := -L. -L$(TOPDIR)/lib/static -L$(TOPDIR)/lib/shard
LIBS := -L. -L$(TOPDIR)/lib/ -L$(PROJ_DIR)

LIBS += -lx264 ./lib/static/libx264.a ./lib/shard/libx264.so.152
LIBS += -lpthread  -lrt -ldl -lm

#### 动态库编译——需注意各个库依赖关系，这个可根据pc文件来查询
##如libswscale.pc中 Requires.private: libavutil >= 56.22.100。表示libswscale会依赖于libavutil
##整体依赖关系如：libavutil -> libpostproc -> libswresample -> libswscale 
##				  -> libavcodec -> libavformat -> libavfilter -> libavdevice
LIBS += -lavdevice -lavfilter -lavformat -lavcodec -lswscale -lswresample -lpostproc -lavutil

##################################### 编译和链接参数 ##############################
#CFLAGS   := -c -O2 -Wall -fPIE 
CFLAGS    := -O2 -fPIE -g
CFLAGS	  += $(INC)
CPPFLAGS  := $(CFLAGS) -std=c++11 

export CFLAGS CPPFLAGS INC LIBS out

##################################### 编译过程 ##############################
MAKE	= make

all: ${TRAGET}

# | 表示先执行后面的动作
$(PROJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(PROJ_DIR)
		$(CPP) $(CPPFLAGS) -c $< -o $@

$(PROJ_DIR)/%.o: $(SRC_DIR)/%.c | $(PROJ_DIR)
		$(CC) $(CFLAGS) -c $< -o $@


#		$(CPP) $(CPPFLAGS) -o $@ $(OBJ_FILES) $(LIBS) -lstdc++
#		$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TRAGET): $(OBJ_FILES) | $(OUT_DIR)
		$(CPP) $(CPPFLAGS) -o $@ $^ $(LIBS)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(PROJ_DIR):
	mkdir -p $(PROJ_DIR)

#%.o :%.c
#		$(CC) $(CFLAGS) -c $< -o $@ 

#%.o :%.cpp
#		$(CPP) $(CPPFLAGS) -c $< -o $@

install:
		@mkdir -p out
		#@cp $(TRAGET) $(TOPDIR)/out

clean:
		rm -f $(shell find ./ -name "*.o")
		rm -f $(shell find ./ -name "*.d")
		rm -f $(TRAGET)

disclean:
		rm -f $(shell find ./ -name "*.o")
		rm -f $(shell find ./ -name "*.d")
		rm -f $(TRAGET)
		rm $(OUT_DIR) $(PROJ_DIR) -rf 
.PHONY: all install clean disclean proj out
