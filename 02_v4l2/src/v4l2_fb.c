#include <linux/fb.h>

#include <v4l2_fb.h>	/* V4L2设备操作 */
#include <global.h>

static struct fb_fix_screeninfo fixinfo;
static struct fb_var_screeninfo varinfo;

static int fb_fd = 0;
static int* fbmmap;
typedef unsigned int uint;


/* 1、open打开framebuffer */
int openframebuffer(void)
{
	int ret = 0;
	fb_fd = open("/dev/fb0", O_RDWR);
	if(fb_fd < 0)
	{
		printf("open framebuffer error.\n");
		exit(0);
	}
	printf("open %s \t[ok]\n", "/dev/fb0");

	/* 获取可变参数 */
	printf("-------------info-----------------\n");
	ret = ioctl(fb_fd, FBIOGET_VSCREENINFO, &varinfo);
	if(ret != 0)
	{
		printf("get varinfo error.\n");
		exit(0);
	}
	else
	{
		printf("xres: %d, yres: %d\nxres_virtual: %d\nyres_virtual: %d \
			\nbpp:%d\n",
			varinfo.xres, varinfo.yres, varinfo.xres_virtual, varinfo.yres_virtual,
			varinfo.bits_per_pixel);
	}

	/* 获取固定参数 */
	ret = ioctl(fb_fd, FBIOGET_FSCREENINFO, &fixinfo);
	if(ret != 0)
	{
		printf("get fixinfo error.\n");
		exit(0);
	}
	else
	{
		printf("buflen:%d \nline_length:%d \n",fixinfo.smem_len, fixinfo.line_length);
	}
}

/* 2、映射mmap内存到用户空间 */
int init_fbmmap(void)
{
	fbmmap = (int *)mmap(NULL, fixinfo.smem_len, PROT_READ|PROT_WRITE,
				MAP_SHARED, fb_fd, 0);
	if(NULL == fbmmap)
	{
		printf("mmap memeory error.\n");
		exit(0);
	}
}
/* 绘制背景颜色 */
void fb_drawback(uint height,uint width,uint color)
{
	uint x,y;
	for(x=0; x<height; x++)
	{
		for(y=0; y<width; y++)
		{
			*(fbmmap + x*(varinfo.xres)+y) = color;
		}
	}
}

/* 绘制bmp数据到framebuffer */
void fb_drawbmp(uint height, uint width, unsigned char *pdata)
{
	uint x,y,cnt;
	uint a = height*width*3;
	uint cnt1 = 0;
	uint* p = fbmmap;
	for(x=0; x<height; x++)
	{
		for(y=0; y<width; y++)
		{
			cnt = x*(varinfo.xres)+y;		// LCD像素位置
			cnt1 = (x*width+width-y)*3;		// 图片像素位置,width-y用于解决图像显示左右反了的问题
			*(p+cnt) = (pdata[cnt1]<<0) | (pdata[cnt1+1]<<8) | (pdata[cnt1+2]<<16);
		}
	}
}

void closefb(void)
{
	munmap(fbmmap, fixinfo.smem_len);
	close(fb_fd);
}


/*
 * @description : 画线函数
 * @param - x1 : 线起始点坐标X轴
 * @param - y1 : 线起始点坐标Y轴
 * @param - x2 : 线终止点坐标X轴
 * @param - y2 : 线终止点坐标Y轴
 * @return : 无
 */
void lcd_drawline(unsigned short x1, unsigned short y1,
unsigned short x2, unsigned short y2)
{
	int t;
	int xerr=0, yerr=0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;

	delta_x = x2 - x1;
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;

	if(delta_x>0)
		incx=1;
	else if(delta_x==0)
		incx=0;
	else
	{
		incx = 1;
		delta_x = -delta_x;
	}

	if(delta_y>0)
		incy=1;
	else if(delta_y==0)
		incy=0;
	else
	{
		incy = 1;
		delta_y = -delta_y;
	}


	if(delta_x>delta_y)	
		distance = delta_x; /*选取基本增量坐标轴*/
	else
		distance = delta_y;

	for(t=0; t<=distance+1; t++)
	{
		lcd_draw_point(uRow, uCol, 0xFF);
		xerr += delta_x;
		yerr += delta_y;
		if(xerr>distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if(yerr>distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}


/*
 * @description : 画点函数
 * @param - x : x轴坐标
 * @param - y : y轴坐标
 * @param - color : 颜色值
 * @return : 无
 */
void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color)
{
	*(fbmmap + y*(varinfo.xres) + x) = color;
}

/*
* @description : 读取指定点的颜色值
* @param - x : x轴坐标
* @param - y : y轴坐标
* @return : 读取到的指定点的颜色值
*/
unsigned int lcd_readpoint(unsigned short x,unsigned short y)
{
	return *(fbmmap + y*(varinfo.xres) + x);
}



