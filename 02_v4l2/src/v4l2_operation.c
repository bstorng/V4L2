#define __OPERATE


#include "v4l2_fb.h"	/* V4L2设备操作 */
#include "global.h"



int error_log(char *log)
{
	int exit = 0;
	if(!log)
	{
		printf("%s\n", log);
		exit = -1;
	}
	return exit;
}

/* 初始化v4l2 */
int v4l2_init(int id)
{
	char *log = NULL;
	char devicename[12];;
	sprintf(devicename,"/dev/video%d",id);
	printf("%s", devicename);
	/* 打开摄像头 */
	v4l2_fd = open(devicename, O_RDWR | O_NONBLOCK, 0);
	if(v4l2_fd < 0)
	{
		sprintf(log, "Open %s error", devicename);
		return error_log(log);
	}
	printf("open %s \t[ok]\n", devicename);
	
	return 0;
}

/* 枚举摄像头功能 */
int v4l2_cap(void)
{
	int ret;
	struct v4l2_capability cap;
	memset(&cap, 0x0, sizeof(cap));
	ret = ioctl(v4l2_fd, VIDIOC_QUERYCAP, &cap);
	if(0 > ret)
	{
		return error_log("query cap error");
	}
	else
	{
		printf("============capability============\n");
		printf("driver: %s\ncard: %s\nbus_info: %s\n", cap.driver, cap.card,cap.bus_info);
		printf("version: %d\ncapabilities: %x\n", cap.version, cap.capabilities);
		if((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
			printf("Device: support capture\n");
		if((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
			printf("Device: support stream\n");
	}
	return 0;
}


/* 枚举并设置摄像头功能 */
int v4l2_enum_fmt(void)
{
	struct v4l2_fmtdesc fmtdesc;
	memset(&fmtdesc, 0x0, sizeof(fmtdesc));

	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmtdesc.index = 0;
	printf("Support format \n");
	while(-1 != ioctl(v4l2_fd, VIDIOC_ENUM_FMT, &fmtdesc))
	{
		printf("[%d]:%s\n", fmtdesc.index, fmtdesc.description);
		fmtdesc.index++;
	}
	return 0;
}

/* 设置摄像头功能 */
int v4l2_set_fmt(int v4l2_width, int v4l2_height)
{
	int ret;
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = v4l2_width;
	fmt.fmt.pix.height = v4l2_height;
	switch(pixIndex)
	{
		case 0:
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
			break;
		case 1:
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
			break;
		default:
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
			break;
	}
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;	/* 隔行插入 */
	ret = ioctl(v4l2_fd, VIDIOC_S_FMT, &fmt);
	if(0 > ret)
	{
		return error_log("Unable to set format");
	}
	printf("set format success\n");

	ret = ioctl(v4l2_fd, VIDIOC_G_FMT, &fmt);
	if(0 > ret)
	{
		return error_log("Unable to get format");
	}
	else
	{
		 printf("fmt.type: %d\n",fmt.type);
         printf("pix.pixelformat:\t%c%c%c%c\n",fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF,(fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
         printf("pix.height:\t\t%d\n",fmt.fmt.pix.height);
         printf("pix.width:\t\t%d\n",fmt.fmt.pix.width);
         printf("pix.field:\t\t%d\n",fmt.fmt.pix.field);
	}

	/* 是否支持帧率设置 */
	struct v4l2_streamparm streamparm;
	streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(V4L2_CAP_TIMEPERFRAME & streamparm.parm.capture.capability)
	{
		streamparm.parm.capture.timeperframe.numerator = 1;
		streamparm.parm.capture.timeperframe.denominator = imageFps;//帧率：imageFps
		ret = ioctl(v4l2_fd, VIDIOC_S_PARM, &streamparm);
		if(0 > ret)
		{
			printf("Error:device do not support set fps\n");
		}
	}

	printf("Set format \t[OK]\n");
	return 0;
}

int v4l2_init_buffer(void)
{
	int ret = 0;
	int n_buffers = 0;
	/* 申请缓存 */
	struct v4l2_requestbuffers req;
	memset(&req, 0x0, sizeof(req));
	req.count = videoCount;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(v4l2_fd, VIDIOC_REQBUFS, &req);
	if(ret < 0)
		return error_log("Req buffers error");

	/* 建立内存映射 */
	//struct v4l2_buffer buf;
	

	/* 为buffers动态分配内存空间 */
	buffers = (struct buffer *)calloc(videoCount, sizeof(struct buffer));

	for(n_buffers=0; n_buffers<videoCount; n_buffers++)
	{
		/* 查询分配好的buffer信息在内核空间中的长度length和偏移量offset */
		memset(&buf, 0x0, sizeof(buf));
		buf.index = n_buffers;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf);
		if(ret < 0)
			return error_log("Query buffers error");		

		/* 内存映射 */
		buffers[buf.index].start = mmap(NULL, buf.length, PROT_READ|PROT_WRITE,
								MAP_SHARED, v4l2_fd, buf.m.offset);
		buffers[buf.index].length = buf.length;
		if(MAP_FAILED == buffers[buf.index].start)
			return error_log("map error");
	}
	printf("Memory map \t[OK]\n");

	/* 入队，等待摄像头数据 */
	for(buf.index=0; buf.index<videoCount; buf.index++)
	{
		ret = ioctl(v4l2_fd, VIDIOC_QBUF, &buf);
		if(ret < 0)
			return error_log("Qbuf error");
	}
	return 0;
}

/* 启动捕获 */
int v4l2_streamon(void)
{
	int ret = 0;
	enum v4l2_buf_type v4l2_type;
	v4l2_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(v4l2_fd, VIDIOC_STREAMON, &v4l2_type);
	if(ret < 0)
		return error_log("Start stream error");
	printf("Start stream \t[OK]\n");
	return 0;
}
void yuv422_2_rgb()
{
	unsigned char YUV[4], RGB[6];
	int i, j, k=0;
	unsigned int location = 0;
	for(i=0; i<buffers[buf.index].length; i+=4)
	{
		YUV[0] = starter[i];	// y
		YUV[1] = starter[i+1];	// u
		YUV[2] = starter[i+2];	// y
		YUV[3] = starter[i+3];	// v

		if(YUV[0] < 0)
		{
			RGB[0] = 0;
			RGB[1] = 0;
			RGB[2] = 0;
		}
		else
		{
#if 0
			RGB[0] = YUV[0] + 1.772 * (YUV[1] - 128);	// b
			RGB[1] = YUV[0] - 0.34414 * (YUV[1] - 128) - 0.71414 * (YUV[3] - 128);	// g
			RGB[2] = YUV[0] + 1.402 * (YUV[3] - 128);	// r
#else
			RGB[0] = 1.164*(YUV[0]-16) + 2.017 * (YUV[1] -128);	// b
			RGB[1] = 1.164*(YUV[0]-16) - 0.813 * (YUV[3] - 128) - 0.392 * (YUV[1] -128);	// g
			RGB[2] = 1.164*(YUV[0]-16) + 1.596 * (YUV[3] - 128);	// r
#endif
		}

		if(YUV[2]<0)
		{
			RGB[3] = 0;
			RGB[4] = 0;
			RGB[5] = 0;
		}
		else
		{
#if 0
			RGB[3] = YUV[2] + 1.772 * (YUV[1] - 128);	// b
			RGB[4] = YUV[2] - 0.34414 * (YUV[1] - 128) - 0.71414 * (YUV[3] - 128);	// g
			RGB[5] = YUV[2] + 1.402 * (YUV[3] - 128);	// r
#else
			RGB[3] = 1.164*(YUV[2]-16) + 2.017 * (YUV[1] -128);	// b
			RGB[4] = 1.164*(YUV[2]-16) - 0.813 * (YUV[3] - 128) - 0.392 * (YUV[1] -128);	// g
			RGB[5] = 1.164*(YUV[2]-16) + 1.596 * (YUV[3] - 128);	// r
		
#endif
		}

		for(j=0; j<6; j++)
		{
			if(RGB[j] < 0)
				RGB[j] = 0;
			if(RGB[j] > 255)
				RGB[j] = 255;
		}
		//请记住：扫描行在位图文件中是反向存储的！  
		if(k%(imageWidth*3) == 0)//定位存储位置
		{
			location = (imageHeight-k/(imageWidth*3))*(imageWidth*3);
		}
		bcopy(RGB, newBuf+location+(k%(imageWidth*3)), sizeof(RGB));
		k+=6;	// 因为YUYV是两个RGB共同使用一组UV数据
	}
}

/* 获取视频数据 */
int v4l2_readFrame(void)
{
	int ret = 0;
	struct pollfd pollfd;
	//struct v4l2_buffer buf;

	for(;;)	// 比while(1)效率更高
	{
		memset(&pollfd, 0x0, sizeof(pollfd));
		pollfd.fd = v4l2_fd;
		pollfd.events = POLLIN;
		ret = poll(&pollfd, 1, 800);
		if(-1 == ret)		// 出错
			return error_log("VIDIOC_QBUF fail");
		else if(0 == ret)	// 超时
		{
			printf("poll time out\n");
			continue;
		}
		if(POLLIN & pollfd.revents)	// 接收到数据
		{
			memset(&buf, 0x0, sizeof(buf));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			ret = ioctl(v4l2_fd, VIDIOC_DQBUF, &buf);	// 出队，取出视频数据
			if(0 > ret)
				return error_log("VIDIOC_QBUF fail");
			/* 具体的逻辑 */

			// RGB格式数据
			starter = (unsigned char*)buffers[buf.index].start;
			newBuf = (unsigned char*)calloc((unsigned int)(buffers[buf.index].length*3/2),sizeof(unsigned char));
			if(pixIndex == 1)
				yuv422_2_rgb();

			//printf("Width: %d, Height: %d\n", imageWidth, imageHeight);
			fb_drawbmp(imageHeight, imageWidth, newBuf);
			lcd_drawline(200, 200, 400, 400);

			
			ret = ioctl(v4l2_fd, VIDIOC_QBUF, &buf);	// 重新入队
			if(0 > ret)
				return error_log("VIDIOC_QBUF fail");
			free(newBuf);
		}
	}
	return ret;
}


/* 获取一帧数据 */
int v4l2_get_oneframe(FrameBuffer *frame)
{

}

/* 关闭设备 */
int v4l2_close(void)
{
	int i, ret = 0;
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(v4l2_fd, VIDIOC_STREAMOFF, &type);
	if(ret < 0)
		return error_log("Stop stream error");
	printf("Stop stream \t [OK]\n");

	for(i=0; i<videoCount; i++)
	{
		munmap(buffers[i].start, buffers[i].length);
	}
	close(v4l2_fd);
	return 0;
}

void quit_Hander(int signum)
{
	if(signum == SIGINT)
    {
        printf("catch Ctrl + C, exit normally\n");

		v4l2_close();
		closefb();
    	exit(0);
	}
}


int main(int argc, char *argv[])
{
#if 1
	struct sigaction sigAction;
	sigAction.sa_handler = quit_Hander;
	sigemptyset(&sigAction.sa_mask);
	sigAction.sa_flags = 0;
	sigaction(SIGINT, &sigAction, NULL);
#else
	int signum = SIGINT;
	signal(signum, quit_Hander);
#endif
	if(argc != 2)
	{
		printf("usage:%s [0|1] \n",argv[0]);
		return -1;
	}

	v4l2_init(atoi(argv[1]));
	v4l2_cap();
	v4l2_enum_fmt();
	pixIndex = 0;
	v4l2_set_fmt(imageWidth, imageHeight);
	v4l2_init_buffer();
	v4l2_streamon();

	v4l2_readFrame();
	return 0;
}





