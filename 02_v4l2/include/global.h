#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h> /* getopt_long() */
#include <fcntl.h> /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <signal.h>
#include <poll.h>

#include <linux/videodev2.h>


#define IMAGE_WIDTH		640
#define IMAGE_HEIGHT	480
#define IMAGE_FPS		30

#define videoCount 		4

struct buffer
{
	void *start;				/* 缓存起始地址 */
	unsigned int length;		/* 缓存区长度 */
}*buffers;
struct buffer videobuffer[videoCount];


typedef struct Frame_Buffer
{
	unsigned char *buf;
	int length;
}FrameBuffer;


typedef unsigned int   uint;

#ifdef __OPERATE
unsigned char* starter;		// RGB格式数据
unsigned char* newBuf;		// 保存图片的缓冲区


int v4l2_fd = -1;

int imageWidth = IMAGE_WIDTH;
int imageHeight = IMAGE_HEIGHT;
int imageFps = IMAGE_FPS;

static int pixIndex = 0;

static struct v4l2_buffer buf;

/*======================V4L2======================*/
/* 初始化v4l2 */
int v4l2_init(int id);
/* 枚举摄像头功能 */
int v4l2_cap(void);
/* 枚举并设置摄像头功能 */
int v4l2_enum_fmt(void);
/* 设置摄像头功能 */
int v4l2_set_fmt(int v4l2_width, int v4l2_height);
/* 内存映射 */
int v4l2_init_buffer(void);
/* 启动捕获 */
int v4l2_streamon(void);
/* 获取一帧数据 */
int v4l2_get_oneframe(FrameBuffer *frame);
/* 关闭设备 */
int v4l2_close(void);

/* 格式转换 */
void yuv422_2_rgb();

/*===============================================*/

#else

#endif
#endif
