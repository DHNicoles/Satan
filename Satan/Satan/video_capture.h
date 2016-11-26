/************************************************************************/
/* Read video files and decode, the same as opencv                      */
/************************************************************************/

#ifndef __VIDEO_CAPTURE_H__
#define __VIDEO_CAPTURE_H__

#include "util.h"
namespace ice
{
	typedef unsigned char BYTE;
	struct DecoderData
	{
		AVFormatContext *pFmtCtx;
		AVCodecContext *pCodecCtx;
		AVFrame *pFrame, *pFrameBGR;
		BYTE *buffer;
		AVPacket *packet;
		struct SwsContext *sws_ctx;
		int streamIdx;
		int nFrames;
		int frmIdx;
		int skipped_frame;

	};
	class VideoCapture
	{
	public:
		VideoCapture()
		{
			pdat_ = new DecoderData;
		}
		VideoCapture(const char* fileName);
		~VideoCapture();
		void Open(const char* fileName);
		bool isOpened() { return work_well_; }
		VideoCapture& operator >> (cv::Mat& frame);
	private:
		static int ffmpeg_decoder_init(struct DecoderData *pDat, const char *cFileName);
		static int ffmpeg_decoder_get_frame(struct DecoderData *pDat, cv::Mat &outImg);
		static void ffmpeg_decoder_free(struct DecoderData *pDat);
		static DecoderData *pdat_;
		bool work_well_;
	};

	
}
#endif//__VIDEO_CAPTURE_H__