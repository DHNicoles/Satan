/************************************************************************/
/* Read video files and decode, the same as opencv                      */
/************************************************************************/

#ifndef __VIDEO_CAPTURE_H__
#define __VIDEO_CAPTURE_H__

#include "util.h"
namespace ice
{
	struct DecoderData;
	class VideoCapture
	{
		typedef unsigned char BYTE;
	public:
		VideoCapture(){}
		VideoCapture(const char* fileName);
		~VideoCapture();
		void Open(const char* fileName);
		bool isOpened() { return work_well_; }
		VideoCapture& operator >> (cv::Mat& frame);
	private:
		int ffmpeg_decoder_init(struct DecoderData *pDat, const char *cFileName);
		int ffmpeg_decoder_get_frame(struct DecoderData *pDat, cv::Mat &outImg);
		void ffmpeg_decoder_free(struct DecoderData *pDat);
		DecoderData *pdat_;
		bool work_well_;
	};

	struct DecoderData
	{
		static AVFormatContext *pFmtCtx;
		static AVCodecContext *pCodecCtx;
		static AVFrame *pFrame, *pFrameBGR;
		static BYTE *buffer;
		static AVPacket *packet;
		static struct SwsContext *sws_ctx;
		static int streamIdx;
		static int nFrames;
		static int frmIdx;
		static int skipped_frame;

	};
}
#endif//__VIDEO_CAPTURE_H__