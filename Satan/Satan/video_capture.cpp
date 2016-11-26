#include "video_capture.h"

namespace ice
{
	VideoCapture::VideoCapture(const char * fileName)
	{
		pdat_ = new DecoderData;
		work_well_ = (ffmpeg_decoder_init(pdat_, fileName) == 0);

	}

	VideoCapture::~VideoCapture()
	{
		ffmpeg_decoder_free(pdat_);
		delete pdat_;
		pdat_ = NULL;
	}

	void VideoCapture::Open(const char * fileName)
	{
		work_well_ = (ffmpeg_decoder_init(pdat_, fileName) == 0);
	}

	VideoCapture& VideoCapture::operator >> (cv::Mat & frame)
	{
		// TODO: 在此处插入 return 语句
		if (!pdat_)
		{
			//invalid DecoderData * pointor;
		}
		else if (0 == ffmpeg_decoder_get_frame(pdat_, frame))
		{
			//ffmpeg_decoder_get_frame error;
		}
		return *this;
	}

	int VideoCapture::ffmpeg_decoder_init(DecoderData * pDat, const char * cFileName)
	{
		AVFormatContext *pFmtCtx = NULL;
		AVCodecContext *pCodecCtx;
		AVFrame *pFrame, *pFrameBGR;
		BYTE *buffer;
		AVCodec *pCodec;

		av_register_all();

		// Open video file
		if (avformat_open_input(&pFmtCtx, cFileName, NULL, NULL) != 0)
			return -1; // Couldn't open file

					   // Retrieve stream information
		if (avformat_find_stream_info(pFmtCtx, NULL) < 0)
			return -2; // Couldn't find stream information

					   // Dump information about file onto standard error
					   //av_dump_format(pFmtCtx, 0, cFileName, 0);//输出视频信息

					   // Find the first video stream
		int videoStream = -1;
		for (int i = 0; i < (int)pFmtCtx->nb_streams; i++)
			if (pFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStream = i;
				break;
			}
		if (videoStream == -1)
			return -4; // Didn't find a video stream

		pDat->streamIdx = videoStream;
		// Get a pointer to the codec context for the video stream
		pCodecCtx = pFmtCtx->streams[videoStream]->codec;

		// Find the decoder for the video stream
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		if (pCodec == NULL) {
			printf("Unsupported codec!\n");
			return -5; // Codec not found
		}

		if (pCodec->capabilities & AV_CODEC_CAP_TRUNCATED)
			pCodecCtx->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames

														 //	pCodecCtx->width = 1280;
														 //	pCodecCtx->height = 720;
														 // Open codec
		if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
		{
			printf("Could not open codec\n");
			return(-7);
		}


		// Allocate video frame
		pFrame = av_frame_alloc();

		// Allocate an AVFrame structure
		pFrameBGR = av_frame_alloc();
		if (pFrame == NULL || pFrameBGR == NULL)
			return -8;

		pDat->packet = (AVPacket *)av_malloc(sizeof(AVPacket));
		av_init_packet(pDat->packet);

		// Determine required buffer size and allocate buffer
		int numBytes = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width,
			pCodecCtx->height);
		buffer = (BYTE *)av_malloc(numBytes * sizeof(BYTE));

		// Assign appropriate parts of buffer to image planes in pFrameRGB
		// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
		// of AVPicture
		avpicture_fill((AVPicture *)pFrameBGR, buffer, AV_PIX_FMT_BGR24,
			pCodecCtx->width, pCodecCtx->height);

		// initialize SWS context for software scaling

		pDat->sws_ctx = sws_getContext(pCodecCtx->width,
			pCodecCtx->height,
			pCodecCtx->pix_fmt,
			pCodecCtx->width,
			pCodecCtx->height,
			AV_PIX_FMT_BGR24,
			SWS_BICUBIC,
			NULL,
			NULL,
			NULL
		);

		pDat->frmIdx = 0;
		pDat->skipped_frame = 0; //liukun
		pDat->pFmtCtx = pFmtCtx;
		pDat->pCodecCtx = pCodecCtx;
		pDat->pFrame = pFrame;
		pDat->pFrameBGR = pFrameBGR;
		pDat->buffer = buffer;

		return 0;
		return 0;
	}
	int VideoCapture::ffmpeg_decoder_get_frame(DecoderData * pDat, cv::Mat & outImg)
	{
		AVFrame *pFrame = pDat->pFrame;
		AVFrame *pFrameBGR = pDat->pFrameBGR;

		AVCodecContext *pCodecCtx = pDat->pCodecCtx;

		int rval; int frameFinished;

		while ((rval = av_read_frame(pDat->pFmtCtx, pDat->packet)) >= 0)
		{
			// Is this a packet from the video stream?
			if (pDat->packet->stream_index != pDat->streamIdx)
				continue;
			// Decode video frame

			avcodec_decode_video2(pCodecCtx, pDat->pFrame, &frameFinished, pDat->packet);
			//(video_dec_ctx, vframe, &got_frame, &packet)
			if (!frameFinished)
			{
				pDat->skipped_frame++;
				continue;
			}
			sws_scale(pDat->sws_ctx, (uint8_t const * const *)pFrame->data,
				pFrame->linesize, 0, pCodecCtx->height,
				pFrameBGR->data, pFrameBGR->linesize);


			if (outImg.empty())
			{
				outImg.create(pCodecCtx->height, pCodecCtx->width, CV_8UC3);
			}

			for (int i = 0; i < (int)outImg.rows; i++)
			{
				BYTE *in = pFrameBGR->data[0] + i*pFrameBGR->linesize[0];
				BYTE *out = outImg.ptr<BYTE>(i);
				for (int j = 0; j < (int)outImg.cols * 3; j += 8)
				{
					out[j + 0] = in[j + 0];
					out[j + 1] = in[j + 1];
					out[j + 2] = in[j + 2];
					out[j + 3] = in[j + 3];
					out[j + 4] = in[j + 4];
					out[j + 5] = in[j + 5];
					out[j + 6] = in[j + 6];
					out[j + 7] = in[j + 7];
				}// for j
			}
			pDat->frmIdx++;
			av_free_packet(pDat->packet);
			//                      printf("get mat op1\n");
			return 0;

		}
		return rval;
		return 0;
	}
	void VideoCapture::ffmpeg_decoder_free(DecoderData * pDat)
	{
		// Free the RGB image
		av_free(pDat->buffer);
		av_frame_free(&(pDat->pFrameBGR));

		// Free the YUV frame
		av_frame_free(&(pDat->pFrame));

		// Free the packet that was allocated by av_read_frame
		av_free_packet(pDat->packet);

		// Close the codecs
		avcodec_close(pDat->pCodecCtx);

		// Close the video file
		avformat_close_input(&(pDat->pFmtCtx));

		// Free scaling context
		sws_freeContext(pDat->sws_ctx);
	}
}
