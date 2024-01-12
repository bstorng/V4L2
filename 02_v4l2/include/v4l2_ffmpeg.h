#ifndef __V4L2_FFMPEG_H__
#define __V4L2_FFMPEG_H__

#if 0
extern "C"   //ffmpeg使用c语言实现的，引入用c写的代码就要用extern
{
#include <libavcodec/avcodec.h>   //注册
#include <libavdevice/avdevice.h>  //设备
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>


#include <v4l2_operation.h>
#include <v4l2_fb.h>
}
#include <iostream>
#include <string>
using namespace std;

class v4l2_ffmpeg
{
public:
	v4l2_ffmpeg();
	~v4l2_ffmpeg();

private:
	AVFormatContext *inputFormatContext, *outputFormatContext;
	AVCodecContext *inputCodecContext, *outputCodecContext;
	AVCodec *inputCodec, *outputCodec;
};
#else




#endif
#endif // __V4L2_FFMPEG_H__