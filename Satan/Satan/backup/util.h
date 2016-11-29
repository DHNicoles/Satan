/************************************************************************/
/* tools                                                                      */
/************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include "opencv2/contrib/contrib.hpp"
#include <string>
#include <vector>     
#include <algorithm>
#include <assert.h>
#include <caffe/caffe.hpp>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <utility>
#include <glog/logging.h>
#include "watch_time.h"
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>  
}

namespace ice
{
	class KernelAlg;

	/************************************************************************/
	/* 
	filepath	:	read file names from 
	videoList	:	save file name list
	*/
	/************************************************************************/
	int  getFileList(std::string filepath, std::vector<std::string> &videoList);
	/************************************************************************/
	/* parse or paramter                                                    */
	/************************************************************************/
	void parseOrDie(const char* configPath, KernelAlg& alg, std::vector<std::string>& file_list);
	int Distance(cv::Rect& r1, cv::Rect& r2);
	int Distance(cv::Rect& r1, cv::Point& pos);
	int Distance(cv::Point& pos1, cv::Point& pos2);
}
#endif // __UTIL_H__

/************************************************************************/
/* macro used in this global scope                                      */
/************************************************************************/

//match distance
#define	DIST_HUMAN	40

//shit or true human
#define JUDGE_TIME	7
#define MIN_MATCH	4


//stop move detection
#define MIN_FRAME_LAST	20
#define MIN_MOVE_PIX    15 	

//draw
#define DRAW_DETECTOR false

#define DRAW_TRACKER true 
