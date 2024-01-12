#ifndef __V4L2_FB_H__
#define __V4L2_FB_H__


typedef unsigned int uint;


/*=================frmaebuffer=================*/
int openframebuffer(void);
int init_fbmmap(void);
void fb_drawback(uint height,uint width,uint color);
void fb_drawbmp(uint height, uint width, unsigned char *pdata);
void closefb(void);
void lcd_drawline(unsigned short x1, unsigned short y1,
	unsigned short x2, unsigned short y2);
void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color);

unsigned int lcd_readpoint(unsigned short x,unsigned short y);
/*===============================================*/



#endif // __V4L2_FB_H__